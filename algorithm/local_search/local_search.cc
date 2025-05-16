#include "algorithm/local_search/local_search.h"

std::vector<std::unique_ptr<LocalSearch>> LocalSearchGenerator::GenerateLocalSearchList()
{
  std::vector<std::unique_ptr<LocalSearch>> result_vector;
  // TWO_OPT
  if (this->usable_local_search[LocalSearchEnum::TWO_OPT])
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
  // OR_OPT
  if (this->usable_local_search[LocalSearchEnum::OR_OPT])
  {
  }
  // SWAP
  if (this->usable_local_search[LocalSearchEnum::SWAP])
  {
  }
  // THREE_OPT
  if (this->usable_local_search[LocalSearchEnum::THREE_OPT])
  {
  }
  // RELOCATE_SAME_VEHICLE
  if (this->usable_local_search[LocalSearchEnum::RELOCATE_SAME_VEHICLE])
  {
  }
  // RELOCATE_DIFF_VEHICLE
  if (this->usable_local_search[LocalSearchEnum::RELOCATE_DIFF_VEHICLE])
  {
  }
  // CROSS_EXCHANGE
  if (this->usable_local_search[LocalSearchEnum::CROSS_EXCHANGE])
  {
  }
  // LAMBDA_EXCHANGE
  if (this->usable_local_search[LocalSearchEnum::LAMBDA_EXCHANGE])
  {
  }
  // LNS
  if (this->usable_local_search[LocalSearchEnum::LNS])
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

std::pair<double, const LocalSearch *> FindBestLocalSearch(
    const std::vector<std::unique_ptr<LocalSearch>> &local_search_list,
    const std::vector<std::vector<std::size_t>> &original_node_records,
    const std::vector<std::vector<double>> &cost_matrix)
{
  double best_cost = DBL_MAX;
  const LocalSearch *best_operator = nullptr;

  for (const auto &op : local_search_list)
  {
    std::vector<std::vector<std::size_t>> test_solution;
    op->Apply(original_node_records, test_solution);

    double cost = EvaluateCost(test_solution, cost_matrix); // Need to Do
    if (cost < best_cost)
    {
      best_cost = cost;
      best_operator = op.get();
      op->test_solution = test_solution;
    }
  }

  return {best_cost, best_operator};
}