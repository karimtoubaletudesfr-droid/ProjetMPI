// Complexité temporelle O(n)
// Mémoire :O(n+p).

#include "roundRobinPartitioning.hpp"
std::vector<std::vector<int>> roundRobinPartitioning(const Graphe& graphe) {
    int nprocs;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    std::vector<std::vector<int>> res(nprocs);
    int p;
    for (int ile = 0; ile < graphe.nombres_de_sommets; ile++) {
        p = ile % nprocs;
        res[p].push_back(ile);
    }
    return res;
}