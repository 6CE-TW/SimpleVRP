#include <iostream>

#include "api/osrm_request.h"
#include "api/api_config.h"

#include "solver/common_tools.h"

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