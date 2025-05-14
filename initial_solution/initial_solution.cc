#include <set>

#include "solver.h"
#include "initial_solution/initial_solution.h"

void SimpleVRPSolver::GetInitialSolution()
{
  if (this->initial_solution_strategy == InitialSolutionStrategy::CHEAPEST_NEIGHBOR)
  {
    this->InitialSolutionCheapestNeighbor();
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