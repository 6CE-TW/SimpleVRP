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
  std::vector<bool> _is_middle_node;
  std::size_t _num_of_middle_nodes;
  std::vector<std::size_t> _start_node_indices;
  std::vector<std::size_t> _end_node_indices;
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

    std::size_t num_of_vehicles = parameter.vehicles.size();
    std::size_t num_of_nodes = parameter.destinations.size();
    this->_num_of_vehicles = num_of_vehicles;
    this->_num_of_nodes = num_of_nodes;

    // build cost matrix
    std::vector<std::vector<double>> cost_matrix;
    std::pair<double, double> cost_ratio = parameter.cost_ratio;
    for (std::size_t i = 0; i < num_of_nodes; ++i)
    {
      std::vector<double> cost_row;
      for (std::size_t j = 0; j < num_of_nodes; ++j)
      {
        double weighted_cost = distance_matrix[i][j] * cost_ratio.first + duration_matrix[i][j] * cost_ratio.second;
        cost_row.push_back(weighted_cost);
      }
      cost_matrix.push_back(cost_row);
    }
    this->_cost_matrix = cost_matrix;

    // build name to index map
    std::unordered_map<std::string, std::size_t> name_mapping;
    for (std::size_t i = 0; i < num_of_nodes; ++i)
    {
      const Destination d = parameter.destinations.at(i);
      name_mapping.insert_or_assign(d.name, i);
    }

    // create start and end node list
    // and middle node bool vector
    std::vector<bool> is_middle_node(num_of_nodes, true);
    std::vector<std::size_t> start_node_indices;
    std::vector<std::size_t> end_node_indices;
    for (std::size_t i = 0; i < num_of_vehicles; ++i)
    {
      const Vehicle v = parameter.vehicles.at(i);

      std::unordered_map<std::string, std::size_t>::iterator iter;
      std::size_t location_start_index = 0;
      if ((iter = name_mapping.find(v.location_start)) != name_mapping.end())
      {
        location_start_index = name_mapping[v.location_start];
      }
      else
      {
        location_start_index = 0;
      }
      start_node_indices.push_back(location_start_index);
      is_middle_node[location_start_index] = false;

      std::size_t location_end_index;
      if ((iter = name_mapping.find(v.location_end)) != name_mapping.end())
      {
        location_end_index = name_mapping[v.location_end];
      }
      else
      {
        location_end_index = num_of_nodes - 1;
      }
      end_node_indices.push_back(location_end_index);
      is_middle_node[location_end_index] = false;
    }
    this->_is_middle_node = is_middle_node;
    this->_start_node_indices = start_node_indices;
    this->_end_node_indices = end_node_indices;

    std::size_t num_of_middle_nodes = 0;
    for (const bool &n : is_middle_node)
    {
      if (n == true)
      {
        num_of_middle_nodes += 1;
      }
    }
    this->_num_of_middle_nodes = num_of_middle_nodes;

    this->route_records = std::vector<std::vector<Route>>(parameter.vehicles.size(), std::vector<Route>{});
  }

  void SetUsableLocalSearch(std::unordered_map<LocalSearchEnum, bool> local_search_list);

  void Solve();
  void PrintSolution();
  void PrintNodeRecords();
  Solution ExtractSolution();
};

#endif // SOLVER_H