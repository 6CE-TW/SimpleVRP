#ifndef SOLVER_H
#define SOLVER_H

#include <vector>
#include "algorithm/initial_solution/initial_solution.h"
#include "algorithm/local_search/metaheuristic.h"
#include "algorithm/common_tools.h"
#include "algorithm/parameter/parameter.h"

struct Route
{
  std::size_t prev;
  std::size_t next;
  std::size_t vehicle;
};

class SimpleVRPSolver
{
private:
  std::vector<std::vector<double>> _cost_matrix;
  std::size_t _num_of_vehicles;
  std::size_t _num_of_nodes;
  double _cost;
  InitialSolutionStrategy initial_solution_strategy = InitialSolutionStrategy::CHEAPEST_NEIGHBOR_MULTIPLE_VEHICLE;
  LocalSearchParameter local_search_parameter;
  MetaheuristicStrategy metaheuristic_strategy = MetaheuristicStrategy::GREEDY;

  std::vector<std::vector<Route>> route_records;
  std::vector<std::vector<std::size_t>> node_records;

  int64_t solve_start_unixtime = 0;

  inline void RecordSolverStartTime()
  {
    this->solve_start_unixtime = now_unixtime_milliseconds();
  }

  void InitialSolutionCheapestNeighbor();
  void InitialSolutionCheapestNeighborMultipleVehicle();
  void GetInitialSolution();

  void EncodeRouteToNodeRecord();

  void PerformLocalSearchOnce();
  void PerformLocalSearchGreedy();
  void PerformLocalSearchGuidedLocalSearch(std::atomic<bool> &time_flag);
  void PerformLocalSearch();

public:
  SimpleVRPSolver(std::vector<std::vector<double>> cost_matrix, std::size_t num_of_vehicles)
  {
    this->_cost_matrix = cost_matrix;
    this->_num_of_vehicles = num_of_vehicles;
    this->_num_of_nodes = cost_matrix.size();
    this->route_records = std::vector<std::vector<Route>>(num_of_vehicles, std::vector<Route>{});
  }

  void SetUsableLocalSearch(std::unordered_map<LocalSearchEnum, bool> local_search_list);

  void Solve();
  void PrintSolution();
  void PrintNodeRecords();
};

#endif // SOLVER_H