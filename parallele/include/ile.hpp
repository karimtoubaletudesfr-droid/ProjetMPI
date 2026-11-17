#ifndef PARALLELE_ILE_HPP
#define PARALLELE_ILE_HPP

#include "habitant.hpp"

struct ile {
    int num;
    char nom[10];
    int nb_hab;
    struct habitant** liste_hab;
    float gini;
    int surface;
    float inflation;
};

void generer_habitants(ile& mon_ile, unsigned int seed_globale);
void affichage_ile(struct ile* ile);
void liberer_habitants(ile& mon_ile);

#endif
