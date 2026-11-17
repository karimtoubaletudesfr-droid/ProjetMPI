#include "../include/monde.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>

struct monde* generation(const char* filename) {
    struct monde* m = (struct monde*)std::malloc(sizeof(struct monde));

    std::ifstream f(filename);

    f >> m->nb_iles;
    int nb = m->nb_iles;
    m->mes_iles = (struct ile**)std::malloc(m->nb_iles * sizeof(struct ile*));

    for (int i = 0; i < nb; i++) {
        m->mes_iles[i] = (struct ile*)std::malloc(sizeof(struct ile));
        f >> (m->mes_iles[i])->num >> (m->mes_iles[i])->nom >> (m->mes_iles[i])->nb_hab;
        m->mes_iles[i]->liste_hab = (struct habitant**)std::malloc(m->mes_iles[i]->nb_hab * sizeof(struct habitant*));
        for (int j = 0; j < m->mes_iles[i]->nb_hab; j++)
            m->mes_iles[i]->liste_hab[j] = creation_habitant(m->mes_iles[i]->nom, j);
    }

    m->mat_adj = (int*)std::calloc(nb * nb, sizeof(int));

    int nb_arete;
    f >> nb_arete;

    int a, b;
    for (int i = 0; i < nb_arete; i++) {
        f >> a >> b;
        m->mat_adj[a * nb + b] = 1;
        m->mat_adj[b * nb + a] = 1;
    }

    m->inflation = inflation_initiale;

    return m;
}

void affichage_monde(struct monde* m) {
    std::cout << m->nb_iles << " iles dans ce monde :" << std::endl;
    for (int i = 0; i < m->nb_iles; i++) affichage_ile(m->mes_iles[i]);
    std::cout << "connectivité : " << std::endl;
    for (int i = 0; i < m->nb_iles; i++) {
        for (int j = 0; j < m->nb_iles; j++) std::cout << m->mat_adj[i * m->nb_iles + j] << " ";
        std::cout << std::endl;
    }
}

void liberer_monde(struct monde** m) {
    for (int i = 0; i < (*m)->nb_iles; i++) {
        std::free((*m)->mes_iles[i]);
        (*m)->mes_iles[i] = NULL;
    }
    std::free((*m)->mes_iles);
    (*m)->mes_iles = NULL;
    std::free((*m)->mat_adj);
    (*m)->mat_adj = NULL;
    std::free(*m);
    *m = NULL;
}
