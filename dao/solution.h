#include <string>
#include <vector>
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

  std::int64_t total_transit_distance()
  {
    std::int64_t sum = 0;
    for (std::size_t i = 0; i < this->tasks.size(); ++i)
    {
      sum += this->tasks[i].transit_distance;
    }

    return sum;
  };

  std::int64_t total_transit_time()
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
};

#endif
