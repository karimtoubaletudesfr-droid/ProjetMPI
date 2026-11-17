#include "../repartition_de_base/graphe.hpp"
#include "../repartition_de_base/roundRobin/roundRobinPartitioning.hpp"

#include "include/monde.hpp"
#include "include/tools.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <mpi.h>
#include <vector>

const int NUMBER_OF_ARGS_IS_NOT_CORRECT = 1;
const int ROOT_RANK_NOT_AVILABLE = 2;
const int NUMBER_OF_YEARS_TO_SIMULATE_IS_NOT_POSITVE = 3;
const int FILE_NOT_FOUND = 4;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    unsigned int seed_globale;
    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    if (argc != 4 && argc != 5) {
        if (rank == 0) {
            std::cerr << "Le nombre de paramètres est incorrect.\n";
            std::cerr << "Utilisation : ./monde_mpi <fichier> <root> <annees> [seed]\n";
        }
        MPI_Abort(MPI_COMM_WORLD, NUMBER_OF_ARGS_IS_NOT_CORRECT);
    }

    const char* fichier = argv[1];
    int root = std::atoi(argv[2]);
    int annees = std::atoi(argv[3]);

    if (root < 0 || root >= nprocs) {
        if (rank == 0) { std::cerr << "Le rang root est invalide\n"; }
        MPI_Abort(MPI_COMM_WORLD, ROOT_RANK_NOT_AVILABLE);
    }

    if (annees <= 0) {
        if (rank == 0) { std::cerr << "Le nombre d'annees doit etre strictement positif.\n"; }
        MPI_Abort(MPI_COMM_WORLD, NUMBER_OF_YEARS_TO_SIMULATE_IS_NOT_POSITVE);
    }
    if (argc == 5) {
        seed_globale = std::atoi(argv[4]);
    } else {
        seed_globale = std::time(nullptr);
    }

    int taille_local = 0;
    std::vector<IlePortable> mes_iles;

    if (rank == root) {
        Graphe graphe;

        if (!initGraphe(fichier, graphe)) { MPI_Abort(MPI_COMM_WORLD, FILE_NOT_FOUND); }

        std::vector<std::vector<int>> partitions = roundRobinPartitioning(graphe);

        std::vector<std::vector<IlePortable>> partitions_iles = preparer_iles(graphe, partitions);

        // Distribution des tailles des partitions
        for (int p = 0; p < nprocs; p++) {
            int taille = partitions_iles[p].size();

            if (p == root) {
                taille_local = taille;
            } else {
                MPI_Send(&taille, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
            }
        }

        // Distribution des iles
        for (int p = 0; p < nprocs; p++) {
            if (p != root) {
                MPI_Send(partitions_iles[p].data(), partitions_iles[p].size() * sizeof(IlePortable), MPI_BYTE, p, 1,
                         MPI_COMM_WORLD);
            }
        }

        // Le root conserve ses propres iles
        mes_iles = partitions_iles[root];

    } else {
        // Reception de la taille de la partition
        MPI_Recv(&taille_local, 1, MPI_INT, root, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Allocation du buffer
        mes_iles.resize(taille_local);

        // Reception des iles
        MPI_Recv(mes_iles.data(), taille_local * sizeof(IlePortable), MPI_BYTE, root, 1, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
    }
    std::vector<ile> mes_iles_locales;
    mes_iles_locales.reserve(mes_iles.size());

    for (const IlePortable& ile_portable : mes_iles) {
        ile nouvelle_ile = {};

        nouvelle_ile.num = ile_portable.num;
        nouvelle_ile.nb_hab = ile_portable.nb_hab;

        std::strcpy(nouvelle_ile.nom, ile_portable.nom);

        nouvelle_ile.gini = 0.0f;
        nouvelle_ile.surface = 0;
        nouvelle_ile.inflation = inflation_initiale;

        generer_habitants(nouvelle_ile, seed_globale);

        mes_iles_locales.push_back(nouvelle_ile);
    }

    // Code provisoire fourni par la prof
    struct monde* mm = generation("fichierMonde/monde1.txt");

    affichage_monde(mm);

    liberer_monde(&mm);

    MPI_Finalize();
    return EXIT_SUCCESS;
}