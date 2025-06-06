#include <string>
#include "solver/config.h"

#ifndef NLOHMANN_JSON_H_
#include <nlohmann/json.hpp>
#define NLOHMANN_JSON_H_
#endif

#ifndef DAO_VEHICLE_H_
#define DAO_VEHICLE_H_

using json = nlohmann::json;

class Vehicle
{
public:
  std::string id = "";

  std::string location_start = "";
  std::string location_end = "";

  Vehicle() {};

  json ToJson() const;
};

inline json Vehicle::ToJson() const
{
  json data;

  data["id"] = this->id;
  data["location"] = {this->location_start, this->location_end};

  return data;
}

static const Vehicle DummyVehicle = Vehicle();

#endif