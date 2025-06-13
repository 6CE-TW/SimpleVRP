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

inline std::string MetaheuristicStrategyEnumToString(MetaheuristicStrategy metaheuristic_strategy)
{
  if (metaheuristic_strategy == MetaheuristicStrategy::DO_NOTHING)
  {
    return "DO_NOTHING";
  }
  if (metaheuristic_strategy == MetaheuristicStrategy::GREEDY)
  {
    return "GREEDY";
  }
  if (metaheuristic_strategy == MetaheuristicStrategy::EPSILON_GREEDY)
  {
    return "EPSILON_GREEDY";
  }
  if (metaheuristic_strategy == MetaheuristicStrategy::GUIDED_LOCAL_SEARCH)
  {
    return "GUIDED_LOCAL_SEARCH";
  }
  if (metaheuristic_strategy == MetaheuristicStrategy::TABU_SEARCH)
  {
    return "TABU_SEARCH";
  }
  if (metaheuristic_strategy == MetaheuristicStrategy::SIMULATED_ANNEALING_SEARCH)
  {
    return "SIMULATED_ANNEALING_SEARCH";
  }
  return "";
}

inline const MetaheuristicStrategy IntToMetaheuristicStrategyEnum(int value)
{
  // clang-format off
  switch (value)
  {
    case 1:   return MetaheuristicStrategy::GREEDY;
    case 2:   return MetaheuristicStrategy::EPSILON_GREEDY;
    case 10:  return MetaheuristicStrategy::GUIDED_LOCAL_SEARCH;
    case 11:  return MetaheuristicStrategy::TABU_SEARCH;
    case 12:  return MetaheuristicStrategy::SIMULATED_ANNEALING_SEARCH;

    case 0:
    default: return MetaheuristicStrategy::DO_NOTHING;
  }
  // clang-format on
}

#endif // METAHEURISTIC_H