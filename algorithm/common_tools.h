#ifndef COMMON_TOOLS_H
#define COMMON_TOOLS_H

#include <chrono>

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