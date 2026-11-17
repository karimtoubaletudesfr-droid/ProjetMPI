#ifndef GRAPHE_HPP
#define GRAPHE_HPP

#include <string>
#include <vector>
struct Graphe {
    int nombres_de_sommets;
    int populationTotal;
    std::vector<std::string> nom_iles;
    std::vector<int> population;
    std::vector<std::vector<int>> voisinage;
};
bool initGraphe(const std::string& filename, Graphe& g);
#endif