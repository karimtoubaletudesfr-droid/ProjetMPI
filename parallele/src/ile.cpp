#include "../include/ile.hpp"

#include <iostream>

void generer_habitants(ile& mon_ile, unsigned int seed_globale) {
    srand(seed_globale + mon_ile.num);

    mon_ile.liste_hab = (habitant**)malloc(mon_ile.nb_hab * sizeof(habitant*));

    for (int j = 0; j < mon_ile.nb_hab; j++) { mon_ile.liste_hab[j] = creation_habitant(mon_ile.nom, j); }
}

void affichage_ile(struct ile* ile) {
    std::cout << ile->nom << "->" << ile->nb_hab << " habitants : ";
    for (int i = 0; i < ile->nb_hab; i++) {
        affichage_habitant(ile->liste_hab[i]);
        std::cout << std::endl;
    }
}
void liberer_habitants(ile& mon_ile) {
    if (mon_ile.liste_hab == nullptr) { return; }

    for (int j = 0; j < mon_ile.nb_hab; j++) { free(mon_ile.liste_hab[j]); }

    free(mon_ile.liste_hab);
    mon_ile.liste_hab = nullptr;
}