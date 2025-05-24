#include <vector>
#include <chrono>
#include "algorithm/local_search/local_search.h"

void SwapSubsegments(
    std::vector<std::size_t> &a, size_t a_start, size_t a_end,
    std::vector<std::size_t> &b, size_t b_start, size_t b_end)
{
  std::vector<std::size_t> temp_a(a.begin() + a_start, a.begin() + a_end);
  std::vector<std::size_t> temp_b(b.begin() + b_start, b.begin() + b_end);

  a.erase(a.begin() + a_start, a.begin() + a_end);
  a.insert(a.begin() + a_start, temp_b.begin(), temp_b.end());

  b.erase(b.begin() + b_start, b.begin() + b_end);
  b.insert(b.begin() + b_start, temp_a.begin(), temp_a.end());
}

std::int64_t now_unixtime()
{
  const auto current_now = std::chrono::system_clock::now();
  const auto time = std::chrono::duration_cast<std::chrono::seconds>(current_now.time_since_epoch()).count();

  return time;
}

std::int64_t now_unixtime_milliseconds()
{
  const auto current_now = std::chrono::system_clock::now();
  const auto time = std::chrono::duration_cast<std::chrono::milliseconds>(current_now.time_since_epoch()).count();

  return time;
}