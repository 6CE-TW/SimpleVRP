#include <string>
#include <vector>
#include <sstream>

#include "dao/destination.h"

#ifndef NLOHMANN_JSON_H_
#include <nlohmann/json.hpp>
#define NLOHMANN_JSON_H_
#endif

#ifndef DAO_SOLUTION_H_
#define DAO_SOLUTION_H_

using json = nlohmann::json;

class Task
{
public:
  std::int32_t sequence = -1;
  std::size_t index = 0;
  Destination destination;

  double transit_distance = 0;
  double transit_time = 0;

  Task() {};
  Task(std::int32_t sequence, std::size_t index, Destination destination)
  {
    this->sequence = sequence;
    this->index = index;
    this->destination = destination;
  };

  json ToJson()
  {
    json data;

    data["sequence"] = this->sequence;
    data["name"] = destination.name;
    data["transit_distance"] = transit_distance;
    data["transit_time"] = transit_time;
    data["lat"] = destination.lat;
    data["lon"] = destination.lon;

    return data;
  };
};

class VehicleTaskRoute
{
public:
  // Vehicle vehicle;
  std::size_t vehicle;
  std::vector<Task> tasks;

  VehicleTaskRoute() {};
  VehicleTaskRoute(std::size_t vehicle)
  {
    this->vehicle = vehicle;
  };

  std::int64_t total_transit_distance() const
  {
    std::int64_t sum = 0;
    for (std::size_t i = 0; i < this->tasks.size(); ++i)
    {
      sum += this->tasks[i].transit_distance;
    }

    return sum;
  };

  std::int64_t total_transit_time() const
  {
    std::int64_t sum = 0;
    for (std::size_t i = 0; i < this->tasks.size(); ++i)
    {
      sum += this->tasks[i].transit_time;
    }

    return sum;
  };

  json ToJson()
  {
    json data;

    data["vehicle_id"] = this->vehicle;

    std::vector<json> _tasks(this->tasks.size());
    std::transform(this->tasks.begin(), this->tasks.end(), _tasks.begin(), [](Task t)
                   { return t.ToJson(); });
    data["tasks"] = _tasks;

    data["total_transit_distance"] = this->total_transit_distance();
    data["total_transit_time"] = this->total_transit_time();

    return data;
  };
};

class Solution
{
public:
  std::string algo_name = "";
  std::string version_name = "";
  std::int32_t version_number = 0;

  std::string status = "DEFAULT";
  std::vector<VehicleTaskRoute> vehicle_task_routes;

  Solution() {};

  json ToJson()
  {
    json data;

    data["status"] = this->status;

    std::vector<json> _routes(this->vehicle_task_routes.size());
    std::transform(this->vehicle_task_routes.begin(), this->vehicle_task_routes.end(), _routes.begin(), [](VehicleTaskRoute vr)
                   { return vr.ToJson(); });
    data["vehicle_task_routes"] = _routes;

    data["calculate_info"]["algo_name"] = this->algo_name;
    data["calculate_info"]["version_name"] = this->version_name;
    data["calculate_info"]["version_number"] = this->version_number;

    return data;
  }

  std::string NumToHexString(double number)
  {
    int rounded = std::clamp(static_cast<int>(std::round(number)), 0, 255);
    std::ostringstream ss;
    ss << std::uppercase << std::hex;
    if (rounded < 16)
      ss << '0';
    ss << rounded;
    return ss.str();
  }

  std::vector<std::vector<std::string>> GenerateVehicleColor(int num_of_vehicle, bool calculate_for_line = false)
  {
    std::vector<std::vector<std::string>> output;

    for (int i = 0; i < num_of_vehicle; ++i)
    {
      double hue = i * 360.0 / num_of_vehicle;
      double saturation = 0.5;
      double value = 1.0;
      
      if(calculate_for_line)
      {
        saturation = 1.0;
        value = 0.5;
      }

      int hue_region = static_cast<int>(hue / 60.0) % 6;
      double f = (hue / 60.0) - hue_region;

      double p = value * (1 - saturation);
      double q = value * (1 - f * saturation);
      double t = value * (1 - (1 - f) * saturation);

      double r = 0, g = 0, b = 0;

      // clang-format off
        switch (hue_region) {
          case 0: r = value; g = t; b = p; break;
          case 1: r = q; g = value; b = p; break;
          case 2: r = p; g = value; b = t; break;
          case 3: r = p; g = q; b = value; break;
          case 4: r = t; g = p; b = value; break;
          case 5: r = value; g = p; b = q; break;
        }
      // clang-format on

      std::vector<std::string> color_hex = {
          NumToHexString(r * 255),
          NumToHexString(g * 255),
          NumToHexString(b * 255)};

      output.push_back(color_hex);
    }

    return output;
  }

  json ToGeoJson()
  {
    json data;
    data["type"] = "FeatureCollection";

    std::size_t num_of_vehicle = this->vehicle_task_routes.size();
    std::vector<std::vector<std::string>> vehicle_colors = GenerateVehicleColor(num_of_vehicle);

    json features_json;
    for (std::size_t i = 0; i < num_of_vehicle; ++i)
    {
      const VehicleTaskRoute vehicle_task_route = vehicle_task_routes.at(i);
      std::vector<std::string> vehicle_color = vehicle_colors.at(i);
      std::string color_hex = vehicle_color.at(0) + vehicle_color.at(1) + vehicle_color.at(2);
      json coordinates_list;
      for (std::size_t j = 0; j < vehicle_task_route.tasks.size(); ++j)
      {
        /*
        "type": "Feature",
        "properties": {
            "marker-color": "#0000F0",
            "marker-size": "medium",
            "marker-symbol": "",
            "location": "wsqq5r98v"
        },
        "geometry": {
            "type": "Point",
            "coordinates": [
                121.43418073654175,
                25.002286434173584
            ]
        }
        */
        const Destination destination = vehicle_task_route.tasks.at(j).destination;

        json feature;
        feature["type"] = "Feature";
        feature["properties"]["marker-color"] = "#" + color_hex;
        feature["properties"]["marker-size"] = "medium";
        feature["properties"]["marker-symbol"] = "";
        feature["properties"]["name"] = destination.name;
        feature["geometry"]["type"] = "Point";
        feature["geometry"]["coordinates"] = {destination.lon, destination.lat};
        features_json.push_back(feature);

        coordinates_list.push_back({destination.lon, destination.lat});
      }

      /*
      "type": "Feature",
      "properties": {
          "strokeColor": "#0000F0"
      },
      "style": {
          "fill": "#0000F0"
      },
      "geometry": {
          "type": "LineString",
          "coordinates": [
              [
                  121.43418073654175,
                  25.002286434173584
              ],
              [
                  121.49666547775269,
                  25.057432651519775
              ],
              ...
              [
                  121.43418073654175,
                  25.002286434173584
              ]
          ]
      }
      */

      json line_feature;
      line_feature["type"] = "Feature";
      line_feature["properties"]["strokeColor"] = "#" + color_hex;
      line_feature["style"]["fill"] = "#" + color_hex;
      line_feature["geometry"]["type"] = "LineString";
      line_feature["geometry"]["coordinates"] = coordinates_list;
      features_json.push_back(line_feature);
    }

    data["features"] = features_json;
    return data;
  }

  json ToGetRouteJson()
  {
    json multi_routes_json = json::array();
    std::size_t num_of_vehicle = this->vehicle_task_routes.size();

    for (std::size_t i = 0; i < num_of_vehicle; ++i)
    {
      json route_json;
      const VehicleTaskRoute vehicle_task_route = vehicle_task_routes.at(i);
      for (std::size_t j = 0; j < vehicle_task_route.tasks.size(); ++j)
      {
        const Destination destination = vehicle_task_route.tasks.at(j).destination;
        json feature;
        feature["name"] = destination.name;
        feature["lat"] = destination.lat;
        feature["lon"] = destination.lon;
        route_json.push_back(feature);

      }
      multi_routes_json.push_back(route_json);
    }
    return multi_routes_json;
  }
};

#endif
