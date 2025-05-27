#include <iostream>
#include <vector>

#include <cpr/cpr.h>

#include "data/data.h"
#include "algorithm/solver.h"

int main()
{
  int n = 10;
  Cartesian sand_box = {100, 100};
  std::cout << "[INFO] Sending request to https://httpbin.org/get" << std::endl;
  cpr::Response r = cpr::Get(cpr::Url{"https://httpbin.org/get"});
  std::cout << "[INFO] Request done." << std::endl;

  if (r.error)
  {
    std::cout << "r.error" << std::endl;
    std::cerr << "[ERROR] CPR failed:\n"
              << "  code    = " << static_cast<int>(r.error.code) << "\n"
              << "  message = " << r.error.message << "\n";
  }
  else
  {
    std::cout << "NOT r.error" << std::endl;
    std::cout << "Status code: " << r.status_code << "\n";
    std::cout << "Response body:\n" << r.text << "\n";
  }

  std::vector<Cartesian> node_map = CreateNodeMap(n, sand_box);
  PrintNodeMap(node_map);

  std::vector<std::vector<double>> cost_matrix = CreateCostMatrix(n, node_map);
  // PrintCostMatrix(cost_matrix, 5);

  SimpleVRPSolver simple_vrp_solver(cost_matrix, 4);
  simple_vrp_solver.Solve();
  simple_vrp_solver.PrintSolution();
  simple_vrp_solver.PrintNodeRecords();

  return 0;
}