#include "monde.hpp"
#include <iostream>

int main(void) {
    struct monde* mm = generation("fichierMonde/monde1.txt");

    affichage_monde(mm);

    liberer_monde(&mm);

    return EXIT_SUCCESS;
}
