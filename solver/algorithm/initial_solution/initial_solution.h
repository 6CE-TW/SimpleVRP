#ifndef INITIAL_SOLUTION_H
#define INITIAL_SOLUTION_H

#include <string>
#include "solver/dao/route.h"

enum InitialSolutionStrategy
{
  CHEAPEST_NEIGHBOR = 1,
  CHEAPEST_NEIGHBOR_MULTIPLE_VEHICLE = 2,
  GLOBAL_MINIMAL = 3,
  GLOBAL_MAXIMUM_VARIANCE_FIRST = 4,
};

inline std::string InitialSolutionStrategyEnumToString(InitialSolutionStrategy initial_solution_strategy)
{
  if (initial_solution_strategy == InitialSolutionStrategy::CHEAPEST_NEIGHBOR)
  {
    return "CHEAPEST_NEIGHBOR";
  }
  if (initial_solution_strategy == InitialSolutionStrategy::CHEAPEST_NEIGHBOR_MULTIPLE_VEHICLE)
  {
    return "CHEAPEST_NEIGHBOR_MULTIPLE_VEHICLE";
  }
  if (initial_solution_strategy == InitialSolutionStrategy::GLOBAL_MINIMAL)
  {
    return "GLOBAL_MINIMAL";
  }
  if (initial_solution_strategy == InitialSolutionStrategy::GLOBAL_MAXIMUM_VARIANCE_FIRST)
  {
    return "GLOBAL_MAXIMUM_VARIANCE_FIRST";
  }
  return "";
}

inline const InitialSolutionStrategy IntToInitialSolutionStrategyEnum(int value)
{
  // clang-format off
  switch (value)
  {
    case 1:  return InitialSolutionStrategy::CHEAPEST_NEIGHBOR;
    case 2:  return InitialSolutionStrategy::CHEAPEST_NEIGHBOR_MULTIPLE_VEHICLE;
    case 3:  return InitialSolutionStrategy::GLOBAL_MINIMAL;
    case 4:  return InitialSolutionStrategy::GLOBAL_MAXIMUM_VARIANCE_FIRST;

    case 0:
    default: return InitialSolutionStrategy::CHEAPEST_NEIGHBOR;
  }
  // clang-format on
}

PathRoutes MergePathRoutes(
    const PathRoutes &first,
    const PathRoutes &second,
    const Route &connecting_edge,
    std::size_t assigned_vehicle);

#endif // INITIAL_SOLUTION_H