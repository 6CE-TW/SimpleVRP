#ifndef OSRM_REQUEST_H
#define OSRM_REQUEST_H

#include <vector>

#ifndef CRP_CRP_H_
#include <cpr/cpr.h>
#define CRP_CRP_H_
#endif
#ifndef NLOHMANN_JSON_H_
#include <nlohmann/json.hpp>
#define NLOHMANN_JSON_H_
#endif

#include "solver/wrapper/wrapper.h"

struct OsrmRequestResult
{
  std::vector<std::vector<double>> distance_matrix;
  std::vector<std::vector<double>> duration_matrix;
};

OsrmRequestResult GenerateOsrmRequestResult(Parameter parameter);

#endif