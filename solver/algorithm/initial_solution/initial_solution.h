#ifndef INITIAL_SOLUTION_H
#define INITIAL_SOLUTION_H

#include <string>
#include "solver/dao/route.h"

enum InitialSolutionStrategy
{
  CHEAPEST_NEIGHBOR = 0,
  CHEAPEST_NEIGHBOR_MULTIPLE_VEHICLE = 1,
  GLOBAL_MINIMAL = 2,
};

std::string InitialSolutionStrategyEnumToString(InitialSolutionStrategy initial_solution_strategy);

PathRoutes MergePathRoutes(
    const PathRoutes &first,
    const PathRoutes &second,
    const Route &connecting_edge,
    std::size_t assigned_vehicle);

#endif // INITIAL_SOLUTION_H