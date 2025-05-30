#include <string>
#include <vector>

#ifndef NLOHMANN_JSON_H_
#include <nlohmann/json.hpp>
#define NLOHMANN_JSON_H_
#endif

#ifndef WRAPPER_H_
#define WRAPPER_H_

using json = nlohmann::json;

class Destination
{
public:
  std::string name;
  double lon;
  double lat;

  json ToJson() const;
};

json Destination::ToJson() const
{
  json data;

  data["name"] = this->name;
  data["lat"] = this->lat;
  data["lon"] = this->lon;

  return data;
}

static const Destination DummyDestination = Destination();

struct Parameter
{
  std::vector<Destination> destinations;
};

std::string DumpParameter(Parameter parameter)
{
  json data;

  std::vector<json> destinations(parameter.destinations.size());
  std::transform(parameter.destinations.begin(), parameter.destinations.end(), destinations.begin(), [](Destination d)
                 { return d.ToJson(); });
  data["destinations"] = destinations;

  return data.dump();
};

class ParameterWrapper
{
public:
  static Parameter FromJson(std::string str);
  static Parameter FromJson(nlohmann::json json_obj);

private:
  ParameterWrapper() {}
};

Parameter ParameterWrapper::FromJson(std::string str)
{
  return ParameterWrapper::FromJson(json::parse(str));
};

Parameter ParameterWrapper::FromJson(nlohmann::json json_obj)
{
  auto parameter = Parameter();

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

#endif
