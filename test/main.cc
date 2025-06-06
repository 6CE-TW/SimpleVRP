#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include "api/osrm_request.h"
#include "data/data.h"
#include "solver/algorithm/solver.h"
#include "solver/wrapper/wrapper.h"

#include "solver/utilities.h"
#include "solver/config.h"

int main()
{
  int cpr_condition_code = VerifyCprCondition();
  if (cpr_condition_code == 1)
  {
    return 1;
  }

  std::string str;

  std::ifstream reader = std::ifstream("../test/test.json", std::ifstream::in);
  if (reader.is_open())
  {
    std::stringstream strStream;
    strStream << reader.rdbuf();
    str = strStream.str();
  }
  else
  {
    std::cout << "!reader.is_open()\n";
    return 1;
  }

  reader.close();

  Parameter data = ParameterWrapper::FromJson(str);
  // std::cout << DumpParameter(data) << std::endl;
  std::vector<std::vector<double>> distance_matrix;
  std::vector<std::vector<double>> duration_matrix;

  if (USE_DUMMY_DISTANCE_MATRIX)
  {
    srand(time(NULL));
    int n = data.destinations.size();
    Cartesian sand_box = {10000, 10000};
    std::vector<Cartesian> node_map = CreateNodeMap(n, sand_box);
    PrintNodeMap(node_map);

    distance_matrix = CreateCostMatrix(n, node_map);
    duration_matrix = CreateCostMatrix(n, node_map);
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
  }
  else
  {
    OsrmMatrixResult osrm_matrix_result = GenerateOsrmMatrixResult(data);
    distance_matrix = osrm_matrix_result.distance_matrix;
    duration_matrix = osrm_matrix_result.duration_matrix;
  }

  std::cout << "Number of Node: " << data.destinations.size() << "\n";
  SimpleVRPSolver simple_vrp_solver(data, distance_matrix, duration_matrix);

  simple_vrp_solver.Solve();
  simple_vrp_solver.PrintSolution();
  simple_vrp_solver.PrintNodeRecords();

  auto timestamp = std::to_string(now_unixtime());
  auto solution = simple_vrp_solver.ExtractSolution();

  SaveJson("../test/results/test_result_" + timestamp + ".json", solution.ToJson());
  SaveJson("../test/results/test_result_" + timestamp + "_geojson.json", solution.ToGeoJson());

  if (USE_DUMMY_DISTANCE_MATRIX == false)
  {
    return SetNavigationToServer(solution, timestamp);
  }

  return 0;
}