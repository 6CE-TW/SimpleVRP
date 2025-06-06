#ifndef SOLVER_H
#define SOLVER_H

#include <vector>
#include "solver/algorithm/initial_solution/initial_solution.h"
#include "solver/algorithm/local_search/metaheuristic.h"
#include "solver/algorithm/parameter/parameter.h"
#include "solver/dao/solution.h"
#include "solver/wrapper/wrapper.h"

#include "solver/common_tools.h"

struct Route
{
  std::size_t prev;
  std::size_t next;
  std::size_t vehicle;
};

class SimpleVRPSolver
{
private:
  std::vector<std::vector<double>> _distance_matrix;
  std::vector<std::vector<double>> _duration_matrix;
  std::vector<std::vector<double>> _cost_matrix;
  std::size_t _num_of_vehicles;
  std::size_t _num_of_nodes;
  double _cost;
  InitialSolutionStrategy initial_solution_strategy = InitialSolutionStrategy::CHEAPEST_NEIGHBOR_MULTIPLE_VEHICLE;
  LocalSearchParameter local_search_parameter;
  MetaheuristicStrategy metaheuristic_strategy = MetaheuristicStrategy::GUIDED_LOCAL_SEARCH;

  std::vector<std::vector<Route>> route_records;
  std::vector<std::vector<std::size_t>> node_records;

  int64_t solve_start_unixtime = 0;

  Parameter _parameter;

  inline Vehicle vehicle(std::size_t index)
  {
    return this->_parameter.vehicles.at(index);
  }

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
  SimpleVRPSolver(
      Parameter parameter,
      std::vector<std::vector<double>> distance_matrix,
      std::vector<std::vector<double>> duration_matrix)
  {
    this->_parameter = parameter;
    this->_distance_matrix = distance_matrix;
    this->_duration_matrix = duration_matrix;

    std::vector<std::vector<double>> cost_matrix;
    std::pair<double, double> cost_ratio = parameter.cost_ratio;
    for (std::size_t i = 0; i < parameter.destinations.size(); ++i)
    {
      std::vector<double> cost_row;
      for (std::size_t j = 0; j < parameter.destinations.size(); ++j)
      {
        double weighted_cost = distance_matrix[i][j] * cost_ratio.first + duration_matrix[i][j] * cost_ratio.second;
        cost_row.push_back(weighted_cost);
      }
      cost_matrix.push_back(cost_row);
    }

    this->_cost_matrix = cost_matrix;
    this->_num_of_vehicles = parameter.vehicles.size();
    this->_num_of_nodes = parameter.destinations.size();
    this->route_records = std::vector<std::vector<Route>>(parameter.vehicles.size(), std::vector<Route>{});
  }

  void SetUsableLocalSearch(std::unordered_map<LocalSearchEnum, bool> local_search_list);

  void Solve();
  void PrintSolution();
  void PrintNodeRecords();
  Solution ExtractSolution();
};

#endif // SOLVER_H