#ifndef METAHEURISTIC_H
#define METAHEURISTIC_H

#include <string>

enum MetaheuristicStrategy
{
  DO_NOTHING = 0,
  GREEDY = 1,
  EPSILON_GREEDY = 2,

  GUIDED_LOCAL_SEARCH = 10,
  TABU_SEARCH = 11,
  SIMULATED_ANNEALING_SEARCH = 12,
};

std::string MetaheuristicStrategyEnumToString(MetaheuristicStrategy initial_solution_strategy);

#endif // METAHEURISTIC_H