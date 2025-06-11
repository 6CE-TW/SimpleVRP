#ifndef INITIAL_SOLUTION_H
#define INITIAL_SOLUTION_H

#include <string>

enum InitialSolutionStrategy
{
  CHEAPEST_NEIGHBOR = 0,
  CHEAPEST_NEIGHBOR_MULTIPLE_VEHICLE = 1,
  GLOBAL_MINIMAL = 2,
};

std::string InitialSolutionStrategyEnumToString(InitialSolutionStrategy initial_solution_strategy);

#endif // INITIAL_SOLUTION_H