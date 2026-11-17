#ifndef WEIGHTED_GREEDY_GRAPH_PARTITIONING_HPP
#define WEIGHTED_GREEDY_GRAPH_PARTITIONING_HPP

#include "../graphe.hpp"

#include <stack>
#include <utility>
#include <vector>

// Calcul du score d'un sommet pour une partition
int calculer_score(int sommet, int indice_partition, const std::vector<int>& affectation, const Graphe& graphe);

// Recherche du meilleur voisin admissible
int chercher_meilleur_voisin_admissible(int indice_partition, const std::vector<std::vector<int>>& partitions,
                                        const std::vector<int>& charges, const std::vector<int>& affectation,
                                        const Graphe& graphe, double chargeToleree);

// Recherche de la partition ayant la charge minimale
int get_indice_de_partion_avec_charge_minimale(const std::vector<int>& charges);

// Recherche des voisins non attribués
std::vector<int> get_voisins_non_attribues_des_sommets_de(const std::vector<int>& partition,
                                                          const std::vector<int>& affectation, const Graphe& graphe);

// Recherche d'une affectation de secours
std::pair<int, int> chercher_affectation_de_secours(const Graphe& graphe, const std::vector<int>& charges,
                                                    const std::vector<int>& affectation, double chargeToleree);

// Attribution d'un sommet à une partition
void attribuer_sommet(int sommet, int indice_partition, const Graphe& graphe, std::vector<std::vector<int>>& partitions,
                      std::vector<int>& charges, std::vector<int>& affectation, int& cpt);

// Tri des sommets par population
std::stack<int> trierSommetsParPopulation(Graphe& g);

// Algorithme principal
std::vector<std::vector<int>> weightedGreedyGraphPartitioning(Graphe& graphe, double tolerance);

#endif
