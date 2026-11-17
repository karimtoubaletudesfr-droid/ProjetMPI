#include "graphe.hpp"
#include "roundRobin/roundRobinPartitioning.hpp"
#include "weightedGreedyGraphPartitioning/weightedGreedyGraphPartitioning.hpp"

#include <cstdlib>
#include <iostream>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rang, nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rang);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    if (argc != 3) {
        if (rang == 0) { std::cerr << "Utilisation : " << argv[0] << " <fichier_monde> <tolerance>\n"; }

        MPI_Finalize();
        return 1;
    }

    if (rang == 0) {
        std::string fichier = argv[1];
        double tolerance = std::atof(argv[2]);

        Graphe graphe;

        if (!initGraphe(fichier, graphe)) {
            std::cerr << "Erreur de lecture du fichier : " << fichier << "\n";

            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        std::vector<std::vector<int>> partitions = roundRobinPartitioning(graphe);
        // weightedGreedyGraphPartitioning(graphe, tolerance);

        double chargeIdeale = static_cast<double>(graphe.populationTotal) / nprocs;

        std::cout << "\n===== TEST DU PARTITIONNEMENT =====\n";

        std::cout << "Monde : " << fichier << "\n";
        std::cout << "Processus MPI : " << nprocs << "\n";
        std::cout << "Tolerance : " << tolerance << "\n";
        std::cout << "Population totale : " << graphe.populationTotal << "\n";
        std::cout << "Charge ideale : " << chargeIdeale << "\n";

        for (std::size_t p = 0; p < partitions.size(); p++) {
            int population = 0;

            std::cout << "\nPartition " << p << " : ";

            for (int sommet : partitions[p]) {
                std::cout << sommet << " ";
                population += graphe.population[sommet];
            }

            std::cout << "\nPopulation : " << population << "\n";
        }
    }

    MPI_Finalize();

    return 0;
}
