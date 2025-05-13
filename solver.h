#ifndef SOLVER_H
#define SOLVER_H

#include <vector>

enum InitialSolutionStrategy
{
  NaiveInitialSolution = 0,
};

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
  InitialSolutionStrategy initial_solution_strategy = InitialSolutionStrategy::NaiveInitialSolution;
  std::vector<std::vector<Route>> route_records;

  void InitialSolutionNaive();

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
};

#endif // SOLVER_H