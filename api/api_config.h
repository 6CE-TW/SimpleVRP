#ifndef API_CONFIG_H
#define API_CONFIG_H

#include <string>

// Fetch path for test part
const std::string httpbin_test_fetch_path = "https://httpbin.org/get";
const std::string osrm_test_fetch_path = "http://35.194.198.57:8000/test";
const std::string osrm_distance_matrix_fetch_path = "http://35.194.198.57:8000/distance-matrix";

// Fetch path for real usage
const std::string matrix_fetch_path = "http://35.194.198.57:8000/cost-matrix";
const std::string navigation_fetch_path = "http://35.194.198.57:8000/get-multiple-routes";
const std::string set_navigation_path = "http://localhost:8080/set-navigation";

constexpr bool TEST_CPR_WITH_HTTPBIN = false;
constexpr bool TEST_CPR_WITH_OSRM = false;
constexpr bool VERIFY_NAVIGATION_DATA = false;

#endif