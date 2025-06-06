#ifndef API_CONFIG_H
#define API_CONFIG_H

#include <string>

const std::string matrix_fetch_path = "http://35.194.198.57:8000/cost-matrix";
const std::string navigation_fetch_path = "http://35.194.198.57:8000/get-multiple-routes";
const std::string set_navigation_path = "http://localhost:8080/set-navigation";

constexpr bool VERIFY_NAVIGATION_DATA = false;

#endif