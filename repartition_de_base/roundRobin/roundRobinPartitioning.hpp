#ifndef ROUNDROBINPARTITIONING_HPP
#define ROUNDROBINPARTITIONING_HPP
#include "../graphe.hpp"
#include <mpi.h>
#include <vector>

std::vector<std::vector<int>> roundRobinPartitioning(const Graphe& graphe);
#endif