#include "graphe.hpp"
#include <fstream>
#include <iostream>
#include <string>

bool initGraphe(const std::string& filename, Graphe& g) {
    std::ifstream fichier(filename);
    if (!fichier.is_open()) {
        std::cerr << "Erreur lors de l'ouverture du fichier.\n";
        return false;
    }
    // initialisation

    int le_nombre_des_iles;
    fichier >> le_nombre_des_iles;
    g.nombres_de_sommets = le_nombre_des_iles;
    g.populationTotal = 0;
    g.population.resize(le_nombre_des_iles);
    g.nom_iles.resize(le_nombre_des_iles);
    g.voisinage.resize(le_nombre_des_iles, std::vector<int>());
    // on commance le parsing de la partie population
    for (int i = 0; i < le_nombre_des_iles; i++) {
        int indice;
        std::string nom;
        int population;
        fichier >> indice >> nom >> population;
        g.population[indice] = population;
        g.nom_iles[indice] = nom;
        g.populationTotal += g.population[indice];
    }
    // parsing du voisinage
    int nb_des_ponts;
    fichier >> nb_des_ponts;
    for (int i = 0; i < nb_des_ponts; i++) {
        int vi;
        int vj;
        fichier >> vi >> vj;
        g.voisinage[vi].push_back(vj);
        g.voisinage[vj].push_back(vi);
    }

    fichier.close();
    return true;
}