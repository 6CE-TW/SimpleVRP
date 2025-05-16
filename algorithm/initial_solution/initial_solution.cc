#include <set>

#include "algorithm/solver.h"
#include "algorithm/initial_solution/initial_solution.h"

std::string InitialSolutionStrategyEnumToString(InitialSolutionStrategy initial_solution_strategy)
{
  if (initial_solution_strategy == InitialSolutionStrategy::CHEAPEST_NEIGHBOR)
  {
    return "CHEAPEST_NEIGHBOR";
  }
  if (initial_solution_strategy == InitialSolutionStrategy::CHEAPEST_NEIGHBOR_MULTIPLE_VEHICLE)
  {
    return "CHEAPEST_NEIGHBOR_MULTIPLE_VEHICLE";
  }
  return "";
}

void SimpleVRPSolver::GetInitialSolution()
{
  if (this->initial_solution_strategy == InitialSolutionStrategy::CHEAPEST_NEIGHBOR)
  {
    this->InitialSolutionCheapestNeighbor();
  }
  if (this->initial_solution_strategy == InitialSolutionStrategy::CHEAPEST_NEIGHBOR_MULTIPLE_VEHICLE)
  {
    this->InitialSolutionCheapestNeighborMultipleVehicle();
  }
}

void SimpleVRPSolver::InitialSolutionCheapestNeighbor()
{
  std::set<std::size_t> traversed_nodes;
  std::size_t latest_arrival_node = 0;

  for (std::size_t i = 1; i < this->_num_of_nodes - 1; ++i)
  {
    std::size_t best_idx = 0;
    std::size_t minimal_cost = INT32_MAX;
    for (std::size_t j = 1; j < this->_num_of_nodes - 1; ++j)
    {
      if (traversed_nodes.find(j) != traversed_nodes.end())
      {
        continue;
      }

      if (this->_cost_matrix[latest_arrival_node][j] < minimal_cost)
      {
        best_idx = j;
        minimal_cost = _cost_matrix[latest_arrival_node][j];
      }
    }

    Route temp_route{latest_arrival_node, best_idx, 0};
    this->route_records[0].push_back(temp_route);
    this->_cost += minimal_cost;

    traversed_nodes.insert(best_idx);
    latest_arrival_node = best_idx;
  }
  Route temp_route{latest_arrival_node, this->_num_of_nodes - 1, 0};
  this->route_records[0].push_back(temp_route);
  this->_cost += this->_cost_matrix[latest_arrival_node][this->_num_of_nodes - 1];

  for (std::size_t i = 1; i < this->_num_of_vehicles; ++i)
  {
    Route temp_route{0, this->_num_of_nodes - 1, i};
    this->route_records[i].push_back(temp_route);
    this->_cost += this->_cost_matrix[0][this->_num_of_nodes - 1];
  }
}

void SimpleVRPSolver::InitialSolutionCheapestNeighborMultipleVehicle()
{
  std::set<std::size_t> traversed_nodes;
  std::vector<std::size_t> latest_arrival_node(this->_num_of_vehicles, 0);

  for (std::size_t i = 1; i < this->_num_of_nodes - 1; ++i)
  {
    std::size_t best_idx = 0;
    std::size_t best_vehicle = 0;
    std::size_t minimal_cost = INT32_MAX;

    for (std::size_t j = 1; j < this->_num_of_nodes - 1; ++j)
    {
      if (traversed_nodes.find(j) != traversed_nodes.end())
      {
        continue;
      }

      for (std::size_t k = 0; k < this->_num_of_vehicles; ++k)
      {
        if (this->_cost_matrix[latest_arrival_node[k]][j] < minimal_cost)
        {
          best_idx = j;
          best_vehicle = k;
          minimal_cost = _cost_matrix[latest_arrival_node[k]][j];
        }
      }
    }

    Route temp_route{latest_arrival_node[best_vehicle], best_idx, best_vehicle};
    this->route_records[best_vehicle].push_back(temp_route);
    this->_cost += minimal_cost;

    traversed_nodes.insert(best_idx);
    latest_arrival_node[best_vehicle] = best_idx;
  }

  for (std::size_t i = 0; i < this->_num_of_vehicles; ++i)
  {
    Route temp_route{latest_arrival_node[i], this->_num_of_nodes - 1, i};
    this->route_records[i].push_back(temp_route);
    this->_cost += this->_cost_matrix[latest_arrival_node[i]][this->_num_of_nodes - 1];
  }
}