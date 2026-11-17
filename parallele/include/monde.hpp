#ifndef PARALLELE_MONDE_HPP
#define PARALLELE_MONDE_HPP

#include "ile.hpp"

#define inflation_initiale 2.5

struct monde {
    struct ile** mes_iles;
    int nb_iles;
    int* mat_adj;
    float inflation;
};

struct monde* generation(const char* filename);
void affichage_monde(struct monde* m);
void liberer_monde(struct monde** m);

#endif
