#include <string>
#include <vector>

#include "dao/destination.h"

#ifndef NLOHMANN_JSON_H_
#include <nlohmann/json.hpp>
#define NLOHMANN_JSON_H_
#endif

#ifndef WRAPPER_H_
#define WRAPPER_H_

using json = nlohmann::json;

struct Parameter
{
  std::vector<Destination> destinations;
};

std::string DumpParameter(Parameter parameter);

class ParameterWrapper
{
public:
  static Parameter FromJson(std::string str);
  static Parameter FromJson(nlohmann::json json_obj);

private:
  ParameterWrapper() {}
};

json ParameterToJsonDict(const Parameter &param);

json ParameterToJsonList(const Parameter &param);

#endif
