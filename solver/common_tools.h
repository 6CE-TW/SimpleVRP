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

inline std::string duration_to_string(double duration)
{
  int hour = 0;
  int min = 0;
  int sec = duration;

  if (sec >= 60)
  {
    min = duration / 60;
    sec -= min * 60;
  }
  if (min >= 60)
  {
    hour = min / 60;
    min -= hour * 60;
  }

  std::string time_str;
  if (hour)
  {
    std::string min_str = std::to_string(min);
    if (min < 10)
    {
      min_str = "0" + min_str;
    }
    std::string sec_str = std::to_string(sec);
    if (sec < 10)
    {
      sec_str = "0" + sec_str;
    }
    time_str = std::to_string(hour) + " h " + min_str + " min " + sec_str + " sec";
  }
  else if (min > 0)
  {
    std::string sec_str = std::to_string(sec);
    if (sec < 10)
    {
      sec_str = "0" + sec_str;
    }
    time_str = std::to_string(min) + " min " + sec_str + " sec";
  }
  else
  {
    time_str = std::to_string(sec) + " sec";
  }

  return time_str;
}

#endif // COMMON_TOOLS