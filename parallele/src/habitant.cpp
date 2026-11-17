#include "../include/habitant.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

struct habitant* creation_habitant(char* nom, int suffixe) {
    struct habitant* h = (struct habitant*)std::malloc(sizeof(struct habitant));
    h->age = std::rand() % 50;
    std::sprintf(h->nom, "%d", suffixe);

    std::strcat(h->nom, nom);
    if (h->age < 18) {
        h->revenu = 0.0f;
    } else {
        h->revenu = revenu_min + ((float)std::rand() / (float)RAND_MAX) * (float)(revenu_max - revenu_min);
    }
    return h;
}

void affichage_habitant(struct habitant* h) {
    std::cout << h->nom << " \t" << h->age << " ans \t(" << h->revenu << "€)";
}
