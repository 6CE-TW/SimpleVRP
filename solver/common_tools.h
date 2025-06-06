#ifndef COMMON_TOOLS_H
#define COMMON_TOOLS_H

#include <chrono>
#include <fstream>
#include <sstream>

#ifndef NLOHMANN_JSON_H_
#include <nlohmann/json.hpp>
#define NLOHMANN_JSON_H_
#endif

inline void SaveJson(std::string _save_path, nlohmann::json json)
{
  std::ofstream file(_save_path);
  file.clear();
  file << json.dump();
  file.close();
}

inline std::int64_t now_unixtime()
{
  const auto current_now = std::chrono::system_clock::now();
  const auto time = std::chrono::duration_cast<std::chrono::seconds>(current_now.time_since_epoch()).count();

  return time;
}

inline std::int64_t now_unixtime_milliseconds()
{
  const auto current_now = std::chrono::system_clock::now();
  const auto time = std::chrono::duration_cast<std::chrono::milliseconds>(current_now.time_since_epoch()).count();

  return time;
}

#endif // COMMON_TOOLS