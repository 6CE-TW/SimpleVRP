#ifndef SOLVER_H
#define SOLVER_H

#include <vector>
#include "initial_solution/initial_solution.h"

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

  std::vector<std::vector<Route>> route_records;
  std::vector<std::vector<std::size_t>> node_records;

  void InitialSolutionCheapestNeighbor();
  void InitialSolutionCheapestNeighborMultipleVehicle();
  void GetInitialSolution();

  void EncodeRouteToNodeRecord();

public:
  SimpleVRPSolver(std::vector<std::vector<double>> cost_matrix, std::size_t num_of_vehicles)
  {
    this->_cost_matrix = cost_matrix;
    this->_num_of_vehicles = num_of_vehicles;
    this->_num_of_nodes = cost_matrix.size();
    this->route_records = std::vector<std::vector<Route>>(num_of_vehicles, std::vector<Route>{});
  }

  void Solve();
  void PrintSolution();
  void PrintNodeRecords();
};

#endif // SOLVER_H