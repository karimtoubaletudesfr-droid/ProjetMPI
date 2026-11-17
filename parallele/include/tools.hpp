#ifndef TOOLS_HPP
#define TOOLS_HPP

#include "habitant.hpp"
#include "ile.hpp"
#include <vector>

#include "../../repartition_de_base/graphe.hpp"
#include "IlePortable.hpp"

std::vector<std::vector<IlePortable>> preparer_iles(const Graphe& graphe,
                                                    const std::vector<std::vector<int>>& partitions);

#endif