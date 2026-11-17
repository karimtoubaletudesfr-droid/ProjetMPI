#include "weightedGreedyGraphPartitioning.hpp"
#include <algorithm>
#include <mpi.h>

int calculer_score(int sommet, int indice_partition, const std::vector<int>& affectation, const Graphe& graphe) {
    int score = 0;

    for (int voisin : graphe.voisinage[sommet]) {
        if (affectation[voisin] == indice_partition) score++;
    }

    return score;
}
int chercher_meilleur_voisin_admissible(int indice_partition, const std::vector<std::vector<int>>& partitions,
                                        const std::vector<int>& charges, const std::vector<int>& affectation,
                                        const Graphe& graphe, double chargeToleree) {
    const std::vector<int>& partition = partitions[indice_partition];
    std::vector<int> candidats = get_voisins_non_attribues_des_sommets_de(partition, affectation, graphe);
    int meilleur = -1;
    int meilleurScore = -1;
    for (int sommet : candidats) {
        if (charges[indice_partition] + graphe.population[sommet] <= chargeToleree) {
            int score = calculer_score(sommet, indice_partition, affectation, graphe);

            if (score > meilleurScore) {
                meilleurScore = score;
                meilleur = sommet;
            }
        }
    }

    return meilleur;
}

int get_indice_de_partion_avec_charge_minimale(const std::vector<int>& charges) {
    int min = charges[0];
    int res = 0;
    for (int i = 1; i < charges.size(); i++) {
        if (charges[i] < min) {
            min = charges[i];
            res = i;
        }
    }
    return res;
}

std::vector<int> get_voisins_non_attribues_des_sommets_de(const std::vector<int>& partition,
                                                          const std::vector<int>& affectation, const Graphe& graphe) {
    std::vector<int> candidat;
    std::vector<bool> dejaVu(graphe.nombres_de_sommets, false);

    for (int i = 0; i < partition.size(); i++) {
        int sommet = partition[i];

        for (int j = 0; j < graphe.voisinage[sommet].size(); j++) {
            int voisin = graphe.voisinage[sommet][j];

            if (affectation[voisin] == -1 && !dejaVu[voisin]) {
                candidat.push_back(voisin);
                dejaVu[voisin] = true;
            }
        }
    }

    return candidat;
}
std::pair<int, int> chercher_affectation_de_secours(const Graphe& graphe, const std::vector<int>& charges,
                                                    const std::vector<int>& affectation, double chargeToleree) {
    int meilleurePartition = -1;
    int meilleurSommet = -1;

    double meilleurDepassement = 0;
    double meilleureCharge = 0;

    for (size_t p = 0; p < charges.size(); p++) {
        for (int sommet = 0; sommet < graphe.nombres_de_sommets; sommet++) {
            if (affectation[sommet] == -1) {
                double nouvelleCharge = charges[p] + graphe.population[sommet];

                double depassement = std::max(0.0, nouvelleCharge - chargeToleree);

                if (meilleurSommet == -1 || depassement < meilleurDepassement ||
                    (depassement == meilleurDepassement && nouvelleCharge < meilleureCharge)) {
                    meilleurDepassement = depassement;
                    meilleureCharge = nouvelleCharge;

                    meilleurePartition = p;
                    meilleurSommet = sommet;
                }
            }
        }
    }

    return std::make_pair(meilleurePartition, meilleurSommet);
}
void attribuer_sommet(int sommet, int indice_partition, const Graphe& graphe, std::vector<std::vector<int>>& partitions,
                      std::vector<int>& charges, std::vector<int>& affectation, int& cpt) {
    partitions[indice_partition].push_back(sommet);

    charges[indice_partition] += graphe.population[sommet];

    affectation[sommet] = indice_partition;

    cpt++;
}

std::stack<int> trierSommetsParPopulation(Graphe& g) {
    std::stack<int> indices;

    // j'ai deja std::vector<int> population; qui assoisie a chaque i sa population

    std::vector<int> temp = std::vector<int>(g.nombres_de_sommets, 0);
    for (int i = 1; i < g.nombres_de_sommets; i++) { temp[i] = i; }

    // trie croissant
    std::sort(temp.begin(), temp.end(), [&g](int a, int b) { return g.population[a] < g.population[b]; });

    for (int i = 0; i < g.nombres_de_sommets; i++) { indices.push(temp[i]); }
    return indices;
}

std::vector<std::vector<int>> weightedGreedyGraphPartitioning(Graphe& graphe, double tolerance) {
    int nprocs;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    double chargeIdeale = (double)graphe.populationTotal / nprocs;
    double chargeToleree = chargeIdeale * (1 + tolerance);

    std::vector<std::vector<int>> partitions(nprocs);
    std::vector<int> charges(nprocs, 0);
    std::vector<int> affectation(graphe.nombres_de_sommets, -1);

    int cpt = 0;

    // Initialisation des partions

    std::stack<int> indices = trierSommetsParPopulation(graphe);

    for (int p = 0; p < std::min(nprocs, graphe.nombres_de_sommets); p++) {
        int v = indices.top();
        partitions[p].push_back(v);
        charges[p] += graphe.population[v];
        affectation[v] = p;
        cpt++;
        indices.pop();
    }

    // Phase 2 : croissance gloutonne des partitions
    // À reconstruire avec les nouvelles fonctions auxiliaires

    while (cpt < graphe.nombres_de_sommets) {
        int indice = get_indice_de_partion_avec_charge_minimale(charges);

        int meilleur =
            chercher_meilleur_voisin_admissible(indice, partitions, charges, affectation, graphe, chargeToleree);

        if (meilleur == -1) {
            std::pair<int, int> secours = chercher_affectation_de_secours(graphe, charges, affectation, chargeToleree);

            indice = secours.first;
            meilleur = secours.second;
        }

        attribuer_sommet(meilleur, indice, graphe, partitions, charges, affectation, cpt);
    }
    return partitions;
}