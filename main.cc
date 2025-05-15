#include <iostream>
#include <vector>

#include "data/data.h"
#include "solver.h"

int main()
{
  int n = 10;
  Cartesian sand_box = {100, 100};
  std::vector<Cartesian> node_map = CreateNodeMap(n, sand_box);
  PrintNodeMap(node_map);

  std::vector<std::vector<double>> cost_matrix = CreateCostMatrix(n, node_map);
  PrintCostMatrix(cost_matrix, 5);

  SimpleVRPSolver simple_vrp_solver(cost_matrix, 4);
  simple_vrp_solver.Solve();
  simple_vrp_solver.PrintSolution();
  simple_vrp_solver.PrintNodeRecords();

  return 0;
}