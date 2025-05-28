#include <iostream>
#include <vector>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "data/data.h"
#include "algorithm/solver.h"

int main()
{
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
    try
    {
      // transform returned text to json object
      nlohmann::json j = nlohmann::json::parse(r.text);

      std::cout << "Response JSON:\n"
                << j.dump(2) << "\n";

      // example: get certain data
      if (j.contains("headers") && j["headers"].contains("Host"))
      {
        std::cout << "Host header: " << j["headers"]["Host"] << "\n";
      }
    }
    catch (nlohmann::json::parse_error &e)
    {
      std::cerr << "[ERROR] JSON parse failed: " << e.what() << "\n";
    }
  }

  srand(time(NULL));
  int n = 100;
  Cartesian sand_box = {10000, 10000};
  std::vector<Cartesian> node_map = CreateNodeMap(n, sand_box);
  PrintNodeMap(node_map);

  std::vector<std::vector<double>> cost_matrix = CreateCostMatrix(n, node_map);
  // clang-format off
  // cost_matrix =
  // {
  //   { 0.00, 41.74, 24.48, 27.79, 57.27, 67.07, 13.61,  63.84,  57.03,  70.60,},
  //   {45.30,  0.00, 67.85, 73.86, 52.51, 77.38, 37.15, 105.65,  14.86,  88.65,},
  //   {26.25, 53.11,  0.00, 55.09, 96.04, 87.73, 35.47,  72.10,  62.84,  88.78,},
  //   {32.59, 63.67, 51.21,  0.00, 45.01, 30.54, 35.96,  30.02,  89.10,  24.77,},
  //   {69.14, 49.49, 93.35, 43.97,  0.00, 32.21, 40.01,  75.50,  71.20,  56.02,},
  //   {59.32, 91.95, 68.91, 38.97, 32.00,  0.00, 52.45,  58.23,  76.21,  25.17,},
  //   {15.57, 31.85, 38.34, 32.02, 42.17, 53.58,  0.00,  54.77,  35.51,  53.49,},
  //   {59.87, 91.55, 60.12, 36.10, 65.63, 47.93, 56.36,   0.00, 108.38,  33.33,},
  //   {46.25, 15.19, 67.27, 76.01, 65.11, 79.03, 41.94, 107.12,   0.00, 104.54,},
  //   {69.48, 81.54, 78.25, 28.69, 67.53, 23.25, 57.27,  28.18,  84.30,   0.00,},
  // };
  // clang-format on
  // PrintCostMatrix(cost_matrix, 5);

  SimpleVRPSolver simple_vrp_solver(cost_matrix, 4);
  simple_vrp_solver.Solve();
  simple_vrp_solver.PrintSolution();
  simple_vrp_solver.PrintNodeRecords();

  return 0;
}