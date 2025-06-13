#include <vector>

#ifndef NLOHMANN_JSON_H_
#include <nlohmann/json.hpp>
#define NLOHMANN_JSON_H_
#endif

#ifndef DAO_ROUTE_H_
#define DAO_ROUTE_H_

using json = nlohmann::json;

class Route
{
public:
  std::size_t prev;
  std::size_t next;
  std::size_t vehicle;
  double cost;
};

class PathRoutes
{
public:
  std::vector<Route> routes;
  std::size_t vehicle;
};

#endif