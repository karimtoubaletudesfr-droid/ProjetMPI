#include "../include/tools.hpp"

#include <cstring>
std::vector<std::vector<IlePortable>> preparer_iles(const Graphe& graphe,
                                                    const std::vector<std::vector<int>>& partitions) {
    std::vector<std::vector<IlePortable>> resultat(partitions.size());

    for (size_t p = 0; p < partitions.size(); p++) {
        resultat[p].reserve(partitions[p].size());

        for (int indice : partitions[p]) {
            IlePortable nouvelle_ile = {}; // initialisation des champs au vals par default

            nouvelle_ile.num = indice;
            nouvelle_ile.nb_hab = graphe.population[indice];

            std::strncpy(nouvelle_ile.nom, graphe.nom_iles[indice].c_str(), sizeof(nouvelle_ile.nom) - 1);
            resultat[p].push_back(nouvelle_ile);
        }
    }

    return resultat;
}
