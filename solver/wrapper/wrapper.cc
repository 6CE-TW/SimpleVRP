#include "solver/wrapper/wrapper.h"

std::string DumpParameter(Parameter parameter)
{
  json data;

  std::vector<json> destinations(parameter.destinations.size());
  std::transform(parameter.destinations.begin(), parameter.destinations.end(), destinations.begin(), [](Destination d)
                 { return d.ToJson(); });
  data["destinations"] = destinations;

  return data.dump();
};

Parameter ParameterWrapper::FromJson(std::string str)
{
  return ParameterWrapper::FromJson(json::parse(str));
};

Parameter ParameterWrapper::FromJson(nlohmann::json json_obj)
{
  auto parameter = Parameter();

  // #region common
  if (json_obj.contains("common"))
  {
    auto json_common = json_obj.at("common");

    if (json_common.contains("cost_ratio"))
    {
      const auto json_cost_ratio = json_common.at("cost_ratio");

      const auto dist_ratio = json_cost_ratio.at(0).get<std::int64_t>();
      const auto time_ratio = json_cost_ratio.at(1).get<std::int64_t>();

      parameter.cost_ratio.first = dist_ratio;
      parameter.cost_ratio.second = time_ratio;
    }

    parameter.initial_solution_strategy = json_common.value("initial_solution_strategy", 4);
    if (parameter.initial_solution_strategy < 0)
    {
      parameter.initial_solution_strategy = 0;
    }
    parameter.metaheuristic_strategy = json_common.value("metaheuristic_strategy", 10);
    if (parameter.metaheuristic_strategy < 0)
    {
      parameter.metaheuristic_strategy = 0;
    }
  }
  // #endregion common

  // #region destinations
  if (json_obj.contains("destinations"))
  {
    auto json_destinations = json_obj.at("destinations");

    for (std::size_t i = 0; i < json_destinations.size(); ++i)
    {
      auto elem = json_destinations.at(i);

      auto destination = Destination();
      destination.name = elem.value("name", "");
      destination.lon = elem.at("lon").get<std::double_t>();
      destination.lat = elem.at("lat").get<std::double_t>();

      // append into struct
      parameter.destinations.push_back(destination);
    }
  }
  // #endregion destinations

  // #region vehicles
  if (json_obj.contains("vehicles"))
  {
    auto json_vehicles = json_obj.at("vehicles");

    for (std::size_t i = 0; i < json_vehicles.size(); ++i)
    {
      auto elem = json_vehicles.at(i);

      auto vehicle = Vehicle();
      vehicle.id = elem.value("id", "");

      if (elem.contains("location"))
      {
        auto json_location = elem.at("location");

        vehicle.location_start = json_location.at(0).get<std::string>();
        vehicle.location_end = json_location.at(1).get<std::string>();
      }

      // append into struct
      parameter.vehicles.push_back(vehicle);
    }
  }
  // #endregion vehicles

  return parameter;
};

json ParameterToJsonDict(const Parameter &param)
{
  json j_array = json::array();
  for (const auto &dest : param.destinations)
  {
    j_array.push_back(dest.ToJson());
  }
  return j_array;
}

json ParameterToJsonList(const Parameter &param)
{
  json j_array = json::array();
  for (const auto &dest : param.destinations)
  {
    j_array.push_back({dest.lon, dest.lat});
  }
  return j_array;
}
