#include "local_search/local_search.h"

std::vector<std::unique_ptr<LocalSearch>> LocalSearchGenerator::GenerateLocalSearchList()
{
  std::vector<std::unique_ptr<LocalSearch>> result_vector;
  // 0: TWO_OPT
  if (this->usable_local_search[0] == true)
  {
    for (std::size_t vehicle = 0; vehicle < this->_num_of_vehicle; ++vehicle)
    {
      if (this->_node_records[vehicle].size() <= 3)
      {
        continue;
      }

      std::size_t size = _node_records[vehicle].size();
      for (std::size_t node_i = 1; node_i < size - 1; ++node_i)
      {
        for (std::size_t node_j = node_i + 1; node_j < size - 1; ++node_j)
        {
          auto two_opt_operator = std::make_unique<TwoOpt>();
          two_opt_operator->vehicle = vehicle;
          two_opt_operator->path_start = node_i;
          two_opt_operator->path_end = node_j;
          result_vector.push_back(std::move(two_opt_operator));
        }
      }
    }
  }
  // 1: OR_OPT
  if (this->usable_local_search[1] == true)
  {
  }
  // 2: RELOCATE
  if (this->usable_local_search[2] == true)
  {
  }
  // 3: SWAP
  if (this->usable_local_search[3] == true)
  {
  }
  // 4: CROSS_EXCHANGE
  if (this->usable_local_search[4] == true)
  {
  }
  // 5: THREE_OPT
  if (this->usable_local_search[5] == true)
  {
  }
  // 6: LAMBDA_EXCHANGE
  if (this->usable_local_search[6] == true)
  {
  }
  // 7: LNS
  if (this->usable_local_search[7] == true)
  {
  }
  return result_vector;
}

void VerifyLocalSearchList(const std::vector<std::unique_ptr<LocalSearch>> &local_search_list)
{
  for (const auto &lc : local_search_list)
  {
    lc->Print();
  }
}

double EvaluateCost(const std::vector<std::vector<std::size_t>> &test_solution, const std::vector<std::vector<double>> &cost_matrix)
{
  double total_cost = 0;
  for (const auto &route : test_solution)
  {
    for (std::size_t i = 1; i < route.size(); ++i)
    {
      total_cost += cost_matrix[route[i - 1]][route[i]];
    }
  }
  return total_cost;
}