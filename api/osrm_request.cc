#include "api/osrm_request.h"

OsrmRequestResult GenerateOsrmRequestResult(Parameter parameter)
{
  OsrmRequestResult osrm_request_result;

  json json_search_matrix_input = ParameterToJsonList(parameter);
  // std::cout<<j.dump()<<"\n";

  cpr::Response matrix_response = cpr::Post(
      cpr::Url{"http://35.194.198.57:8000/cost-matrix"},
      cpr::Header{{"Content-Type", "application/json"}},
      cpr::Body{json_search_matrix_input.dump()});

  json matrix_response_json = json::parse(matrix_response.text);
  // Export Distance Matrix
  for (const auto &row : matrix_response_json["distances"])
  {
    std::vector<double> distance_row;
    for (const auto &value : row)
    {
      distance_row.push_back(value.get<double>());
    }
    osrm_request_result.distance_matrix.push_back(distance_row);
  }

  for (const auto &row : matrix_response_json["durations"])
  {
    std::vector<double> duration_row;
    for (const auto &value : row)
    {
      duration_row.push_back(value.get<double>());
    }
    osrm_request_result.duration_matrix.push_back(duration_row);
  }

  return osrm_request_result;
}