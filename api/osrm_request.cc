#include <iostream>

#include "api/osrm_request.h"
#include "api/api_config.h"

#include "solver/common_tools.h"

int VerifyCprCondition()
{
  if (TEST_CPR_WITH_HTTPBIN)
  {
    std::cout << "[INFO] Sending request to https://httpbin.org/get" << std::endl;
    cpr::Response r = cpr::Get(cpr::Url{httpbin_test_fetch_path});
    std::cout << "[INFO] Request done." << std::endl;

    if (r.error)
    {
      std::cout << "r.error" << std::endl;
      std::cerr << "[ERROR] CPR failed:\n"
                << "  code    = " << static_cast<int>(r.error.code) << "\n"
                << "  message = " << r.error.message << "\n";
      return 1;
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
        return 1;
      }
    }
  }
  else if (TEST_CPR_WITH_OSRM)
  {
    std::cout << "[INFO] Sending request to 35.194.198.57:8000/test" << std::endl;
    cpr::Response r1 = cpr::Get(cpr::Url{osrm_test_fetch_path});
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
      return 1;
    }
    // make JSON body to pass
    std::string body = R"([[121.517,25.0477],[121.5168,25.0443]])";

    cpr::Response r2 = cpr::Post(
        cpr::Url{osrm_distance_matrix_fetch_path},
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
      return 1;
    }
  }

  return 0;
}

OsrmMatrixResult GenerateOsrmMatrixResult(Parameter parameter)
{
  OsrmMatrixResult osrm_matrix_result;

  json json_search_matrix_input = ParameterToJsonList(parameter);
  // std::cout<<j.dump()<<"\n";

  cpr::Response matrix_response = cpr::Post(
      cpr::Url{matrix_fetch_path},
      cpr::Header{{"Content-Type", "application/json"}},
      cpr::Body{json_search_matrix_input.dump()});

  json matrix_response_json = json::parse(matrix_response.text);
  // Export Distance Matrix
  for (const auto &row : matrix_response_json["distances"])
  {
    std::vector<double> distance_row;
    for (const auto &value : row)
    {
      distance_row.push_back(value.get<double>());
    }
    osrm_matrix_result.distance_matrix.push_back(distance_row);
  }

  for (const auto &row : matrix_response_json["durations"])
  {
    std::vector<double> duration_row;
    for (const auto &value : row)
    {
      duration_row.push_back(value.get<double>());
    }
    osrm_matrix_result.duration_matrix.push_back(duration_row);
  }

  return osrm_matrix_result;
}

int SetNavigationToServer(Solution solution, std::string timestamp)
{
  json multi_routes_json = solution.ToGetRouteJson();

  // send POST request to server
  cpr::Response res_route = cpr::Post(
      cpr::Url{navigation_fetch_path},
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
      const Task task = vehicle_task_route.tasks.at(j);

      json feature;
      feature["type"] = "Feature";
      feature["properties"]["marker-color"] = "#" + node_color_hex;
      feature["properties"]["marker-size"] = "medium";
      feature["properties"]["name"] = task.destination.name;
      feature["properties"]["serial"] = j;
      feature["properties"]["transit_distance"] = task.transit_distance;
      feature["properties"]["transit_time"] = task.transit_time;
      feature["properties"]["transit_cost"] = task.transit_cost;
      feature["properties"]["vehicle_id"] = vehicle_task_route.vehicle.id;
      feature["geometry"]["type"] = "Point";
      feature["geometry"]["coordinates"] = {task.destination.lon, task.destination.lat};
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
    line_feature["properties"]["vehicle_id"] = vehicle_task_route.vehicle.id;
    line_feature["information"]["distance"] = vehicle_task_route.total_transit_distance();
    line_feature["information"]["duration"] = vehicle_task_route.total_transit_time();
    line_feature["information"]["cost"] = vehicle_task_route.total_transit_cost();
    line_feature["information"]["node_count"] = vehicle_task_route.tasks.size();
    line_feature["geometry"]["type"] = "LineString";
    line_feature["geometry"] = single_navigation_json["routes"][0]["geometry"];
    features_json.push_back(line_feature);
  }
  navigation_geo_json["features"] = features_json;
  SaveJson("../test/results/test_result_" + timestamp + "_navigation.json", navigation_geo_json);

  cpr::Response local_res = cpr::Post(
      cpr::Url{set_navigation_path},
      cpr::Header{{"Content-Type", "application/json"}},
      cpr::Body{navigation_geo_json.dump()});

  return 0;
}