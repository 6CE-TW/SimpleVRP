#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include <cpr/cpr.h>
#ifndef NLOHMANN_JSON_H_
#include <nlohmann/json.hpp>
#define NLOHMANN_JSON_H_
#endif

#include "data/data.h"
#include "algorithm/solver.h"

#include "wrapper/wrapper.h"

const bool TEST_CPR_WITH_HTTPBIN = false;
const bool TEST_CPR_WITH_OSRM = false;
const bool TEST_FROM_DUMMY_DATA = false;
const bool PRINT_DISTANCE_MATRIX = false;
const bool VERIFY_NAVIGATION_DATA = false;

void SaveJson(std::string _save_path, nlohmann::json json)
{
  std::ofstream file(_save_path);
  file.clear();
  file << json.dump();
  file.close();
}

int main()
{
  if (TEST_CPR_WITH_HTTPBIN)
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
  }
  else if (TEST_CPR_WITH_OSRM)
  {
    std::cout << "[INFO] Sending request to 35.194.198.57:8000/test" << std::endl;
    cpr::Response r1 = cpr::Get(cpr::Url{"http://35.194.198.57:8000/test"});
    std::cout << "[INFO] Request done." << std::endl;

    if (r1.status_code != 200)
    {
      std::cerr << "Failed to get data from server: " << r1.status_code << "\n";
      return 1;
    }

    std::cout << "Status code: " << r1.status_code << "\n";
    std::cout << "Response body:\n"
              << r1.text << "\n";
    try
    {
      // transform returned text to json object
      nlohmann::json j1 = nlohmann::json::parse(r1.text);

      std::cout << "Response JSON:\n"
                << j1.dump(2) << "\n";

      // example: get certain data
      if (j1.contains("message"))
      {
        std::cout << "message: " << j1.at("message").get<std::string>() << "\n";
      }
    }
    catch (nlohmann::json::parse_error &e)
    {
      std::cerr << "[ERROR] JSON parse failed: " << e.what() << "\n";
    }

    std::string url = "http://35.194.198.57:8000/distance-matrix";

    // make JSON body to pass
    std::string body = R"([[121.517,25.0477],[121.5168,25.0443]])";

    cpr::Response r2 = cpr::Post(
        cpr::Url{url},
        cpr::Header{{"Content-Type", "application/json"}},
        cpr::Body{body});

    if (r2.status_code == 200)
    {
      auto json = nlohmann::json::parse(r2.text);
      std::cout << json.dump(2) << std::endl;
    }
    else
    {
      std::cerr << "Error: " << r2.status_code << std::endl;
    }
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

  json j = ParameterToJsonList(data);
  // std::cout<<j.dump()<<"\n";

  cpr::Response r = cpr::Post(
      cpr::Url{"http://35.194.198.57:8000/distance-matrix"},
      cpr::Header{{"Content-Type", "application/json"}},
      cpr::Body{j.dump()});

  if (PRINT_DISTANCE_MATRIX)
  {
    if (r.status_code == 200)
    {
      auto res = json::parse(r.text);
      std::cout << res.dump(2) << std::endl;
    }
    else
    {
      std::cerr << "Request failed: " << r.status_code << std::endl;
    }
  }

  std::vector<std::vector<double>> cost_matrix;
  std::size_t num_of_vehicle = data.vehicles.size();
  if (TEST_FROM_DUMMY_DATA)
  {
    srand(time(NULL));
    int n = 100;
    Cartesian sand_box = {10000, 10000};
    std::vector<Cartesian> node_map = CreateNodeMap(n, sand_box);
    PrintNodeMap(node_map);

    cost_matrix = CreateCostMatrix(n, node_map);
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
    auto res = json::parse(r.text);
    // Export Distance Matrix
    std::vector<std::vector<double>> distance_matrix;
    for (const auto &row : res["distances"])
    {
      std::vector<double> distance_row;
      for (const auto &value : row)
      {
        distance_row.push_back(value.get<double>());
      }
      distance_matrix.push_back(distance_row);
    }

    // std::vector<std::vector<double>> duration_matrix;
    // for (const auto &row : res["duration"])
    // {
    //   std::vector<double> duration_row;
    //   for (const auto &value : row)
    //   {
    //     duration_row.push_back(value.get<double>());
    //   }
    //   duration_matrix.push_back(duration_row);
    // }

    // // 2. Take information of each node ("sources" or "destinations", they are symmetric)
    // std::vector<PointInfo> points;
    // for (const auto &p : res["sources"])
    // {
    //   PointInfo info;
    //   info.name = p.value("name", "");
    //   info.lon = p["location"][0].get<double>();
    //   info.lat = p["location"][1].get<double>();
    //   points.push_back(info);
    // }

    if (PRINT_DISTANCE_MATRIX)
    {
      // Print Result
      std::cout << "Distance Matrix: " << std::endl;
      for (size_t i = 0; i < distance_matrix.size(); ++i)
      {
        for (size_t j = 0; j < distance_matrix[i].size(); ++j)
        {
          std::cout << distance_matrix[i][j] << " ";
        }
        std::cout << std::endl;
      }
      std::cout << std::endl;
    }

    // std::cout << "\nLocation Information:" << std::endl;
    // for (size_t i = 0; i < points.size(); ++i)
    // {
    //   std::cout << "Node " << i << ": "
    //             << points[i].name << " "
    //             << "(" << points[i].lat << ", " << points[i].lon << ")"
    //             << std::endl;
    // }

    cost_matrix = distance_matrix;
  }

  std::cout << "Number of Node: " << cost_matrix.size() << "\n";

  SimpleVRPSolver simple_vrp_solver(cost_matrix, num_of_vehicle);

  if (TEST_FROM_DUMMY_DATA == false)
  {
    simple_vrp_solver.SetParameter(data);
  }

  simple_vrp_solver.Solve();
  simple_vrp_solver.PrintSolution();
  simple_vrp_solver.PrintNodeRecords();

  auto timestamp = std::to_string(now_unixtime());
  auto solution = simple_vrp_solver.ExtractSolution();

  SaveJson("../test/results/test_result_" + timestamp + ".json", solution.ToJson());
  SaveJson("../test/results/test_result_" + timestamp + "_geojson.json", solution.ToGeoJson());

  json multi_routes_json = solution.ToGetRouteJson();

  // send POST request to server
  cpr::Response res_route = cpr::Post(
      cpr::Url{"http://35.194.198.57:8000/get-multiple-routes"},
      cpr::Header{{"Content-Type", "application/json"}},
      cpr::Body{multi_routes_json.dump()});

  // Print result
  if (VERIFY_NAVIGATION_DATA)
  {
    if (res_route.status_code == 200)
    {
      std::cout << "Receive route data successfully:" << std::endl;
      std::cout << res_route.text << std::endl;
    }
    else
    {
      std::cerr << "Search failed, HTTP code: " << res_route.status_code << std::endl;
      std::cerr << res_route.text << std::endl;
      return 1;
    }
  }

  json navigation_json = json::parse(res_route.text);

  std::vector<std::vector<std::string>> node_colors = solution.GenerateVehicleColor(solution.vehicle_task_routes.size(), false);
  std::vector<std::vector<std::string>> line_colors = solution.GenerateVehicleColor(solution.vehicle_task_routes.size(), true);
  json navigation_geo_json;
  navigation_geo_json["type"] = "FeatureCollection";

  json features_json = json::array();
  for (std::size_t i = 0; i < navigation_json.size(); ++i)
  {
    const VehicleTaskRoute vehicle_task_route = solution.vehicle_task_routes.at(i);

    std::vector<std::string> line_color = line_colors.at(i);
    std::string line_color_hex = line_color.at(0) + line_color.at(1) + line_color.at(2);
    std::vector<std::string> node_color = node_colors.at(i);
    std::string node_color_hex = node_color.at(0) + node_color.at(1) + node_color.at(2);
    for (std::size_t j = 0; j < vehicle_task_route.tasks.size(); ++j)
    {
      const Destination destination = vehicle_task_route.tasks.at(j).destination;

      json feature;
      feature["type"] = "Feature";
      feature["properties"]["marker-color"] = "#" + node_color_hex;
      feature["properties"]["marker-size"] = "medium";
      feature["properties"]["name"] = destination.name;
      feature["properties"]["serial"] = j;
      feature["properties"]["vehicle_id"] = i + 1;
      feature["geometry"]["type"] = "Point";
      feature["geometry"]["coordinates"] = {destination.lon, destination.lat};
      features_json.push_back(feature);
    }

    json single_navigation_json = navigation_json.at(i);

    json line_feature;
    line_feature["type"] = "Feature";
    line_feature["properties"]["strokeColor"] = "#" + line_color_hex;
    line_feature["style"]["fill"] = "#" + line_color_hex;
    line_feature["properties"]["stroke"] = "#" + line_color_hex;
    line_feature["properties"]["stroke-width"] = 3;
    line_feature["properties"]["stroke-opacity"] = 0.8;
    line_feature["properties"]["vehicle_id"] = i + 1;
    line_feature["information"]["distance"] = vehicle_task_route.total_transit_distance();
    line_feature["information"]["duration"] = vehicle_task_route.total_transit_time();
    line_feature["information"]["node_count"] = vehicle_task_route.tasks.size();
    line_feature["geometry"]["type"] = "LineString";
    line_feature["geometry"] = single_navigation_json["routes"][0]["geometry"];
    features_json.push_back(line_feature);
  }
  navigation_geo_json["features"] = features_json;
  SaveJson("../test/results/test_result_" + timestamp + "_navigation.json", navigation_geo_json);

  cpr::Response local_res = cpr::Post(
      cpr::Url{"http://localhost:8080/set-navigation"},
      cpr::Header{{"Content-Type", "application/json"}},
      cpr::Body{navigation_geo_json.dump()});

  return 0;
}