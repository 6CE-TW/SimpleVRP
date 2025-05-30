#include <string>

#ifndef NLOHMANN_JSON_H_
#include <nlohmann/json.hpp>
#define NLOHMANN_JSON_H_
#endif

#ifndef DESTINATION_H_
#define DESTINATION_H_

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

#endif