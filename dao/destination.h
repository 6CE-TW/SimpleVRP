#include <string>

#ifndef NLOHMANN_JSON_H_
#include <nlohmann/json.hpp>
#define NLOHMANN_JSON_H_
#endif

#ifndef DAO_DESTINATION_H_
#define DAO_DESTINATION_H_

using json = nlohmann::json;

class Destination
{
public:
  std::string name;
  double lon;
  double lat;

  json ToJson() const;
};

inline json Destination::ToJson() const
{
  json data;

  data["name"] = this->name;
  data["lat"] = this->lat;
  data["lon"] = this->lon;

  return data;
}

static const Destination DummyDestination = Destination();

#endif