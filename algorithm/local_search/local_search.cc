#include "algorithm/local_search/local_search.h"
#include "algorithm/config.h"

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
    for (std::size_t vehicle = 0; vehicle < this->_num_of_vehicle; ++vehicle)
    {
      if (this->_node_records[vehicle].size() <= 3)
      {
        continue;
      }

      std::size_t size = _node_records[vehicle].size();
      std::size_t segment_length_limit = size - 3;
      if (OR_OPT_LENGTH_LIMIT != -1)
      {
        segment_length_limit = OR_OPT_LENGTH_LIMIT;
      }

      for (std::size_t segment_length = 1; segment_length <= segment_length_limit; ++segment_length)
      {
        for (std::size_t node = 1; node + segment_length < size; ++node)
        {
          // sub-segment length = 1, (position == node || position == node + 1) continue
          // sub-segment length = 2, (position == node || position == node + 1 || position == node + 2) continue
          // sub-segment length = 3, (position == node || position == node + 1 || position == node + 2 || position == node + 3) continue
          for (std::size_t position = 1; position < size; ++position)
          {
            if (node <= position && position <= node + segment_length)
            {
              continue;
            }
            auto or_opt_operator = std::make_unique<OrOpt>();
            or_opt_operator->vehicle = vehicle;
            or_opt_operator->original_path_position_start = node;
            or_opt_operator->new_path_position = position;
            or_opt_operator->segment_length = segment_length;
            result_vector.push_back(std::move(or_opt_operator));
          }
        }
      }
    }
  }
  // THREE_OPT
  if (this->usable_local_search[LocalSearchEnum::THREE_OPT])
  {
  }
  // RELOCATE_SAME_VEHICLE
  if (this->usable_local_search[LocalSearchEnum::RELOCATE_SAME_VEHICLE])
  {
    for (std::size_t vehicle = 0; vehicle < this->_num_of_vehicle; ++vehicle)
    {
      if (this->_node_records[vehicle].size() <= 3)
      {
        continue;
      }

      std::size_t size = _node_records[vehicle].size();
      for (std::size_t node = 1; node < size - 1; ++node)
      {
        for (std::size_t position = 1; position < size; ++position)
        {
          if (position == node || position == node + 1)
          {
            continue;
          }
          auto relocate_operator = std::make_unique<RelocateSameVehicle>();
          relocate_operator->vehicle = vehicle;
          relocate_operator->original_path_position = node;
          relocate_operator->new_path_position = position;
          result_vector.push_back(std::move(relocate_operator));
        }
      }
    }
  }
  // RELOCATE_DIFF_VEHICLE
  if (this->usable_local_search[LocalSearchEnum::RELOCATE_DIFF_VEHICLE])
  {
    for (std::size_t vehicle_i = 0; vehicle_i < this->_num_of_vehicle; ++vehicle_i)
    {
      if (this->_node_records[vehicle_i].size() <= 2)
      {
        continue;
      }

      std::size_t size = _node_records[vehicle_i].size();
      for (std::size_t node = 1; node < size - 1; ++node)
      {
        for (std::size_t vehicle_j = 0; vehicle_j < this->_num_of_vehicle; ++vehicle_j)
        {
          if (vehicle_i == vehicle_j)
          {
            continue;
          }
          std::size_t size_position = _node_records[vehicle_j].size();
          for (std::size_t position = 1; position < size_position; ++position)
          {
            auto relocate_operator = std::make_unique<RelocateDiffVehicle>();
            relocate_operator->original_vehicle = vehicle_i;
            relocate_operator->new_vehicle = vehicle_j;
            relocate_operator->original_path_position = node;
            relocate_operator->new_path_position = position;
            result_vector.push_back(std::move(relocate_operator));
          }
        }
      }
    }
  }
  // EXCHANGE
  if (this->usable_local_search[LocalSearchEnum::EXCHANGE])
  {
    for (std::size_t vehicle_i = 0; vehicle_i < this->_num_of_vehicle; ++vehicle_i)
    {
      // if (this->_node_records[vehicle_i].size() <= 2)
      // {
      //   continue;
      // }

      std::size_t size_i = _node_records[vehicle_i].size();
      for (std::size_t node_i = 1; node_i < size_i - 1; ++node_i)
      {
        for (std::size_t vehicle_j = vehicle_i; vehicle_j < this->_num_of_vehicle; ++vehicle_j)
        {
          // if (this->_node_records[vehicle_j].size() <= 2)
          // {
          //   continue;
          // }

          std::size_t size_j = _node_records[vehicle_j].size();
          for (std::size_t node_j = 1; node_j < size_j - 1; ++node_j)
          {
            if (vehicle_i == vehicle_j && node_j <= node_i)
            {
              continue;
            }

            auto exchange_operator = std::make_unique<Exchange>();
            exchange_operator->vehicle_i = vehicle_i;
            exchange_operator->vehicle_j = vehicle_j;
            exchange_operator->path_position_i = node_i;
            exchange_operator->path_position_j = node_j;
            result_vector.push_back(std::move(exchange_operator));
          }
        }
      }
    }
  }
  // CROSS
  if (this->usable_local_search[LocalSearchEnum::CROSS])
  {
    for (std::size_t vehicle_i = 0; vehicle_i < this->_num_of_vehicle; ++vehicle_i)
    {
      std::size_t size_i = _node_records[vehicle_i].size();
      // all sub-segment [segment_i_start, segment_i_end) of route_i
      for (std::size_t segment_i_start = 1; segment_i_start < size_i - 1; ++segment_i_start)
      {
        for (std::size_t segment_i_end = segment_i_start + 1; segment_i_end <= size_i - 1; ++segment_i_end)
        {
          size_t length_i = segment_i_end - segment_i_start;
          if (CROSS_LENGTH_LIMIT != -1 && length_i > CROSS_LENGTH_LIMIT)
            break;

          for (std::size_t vehicle_j = vehicle_i + 1; vehicle_j < this->_num_of_vehicle; ++vehicle_j)
          {
            std::size_t size_j = _node_records[vehicle_j].size();
            for (std::size_t segment_j_start = 1; segment_j_start < size_j - 1; ++segment_j_start)
            {
              for (std::size_t segment_j_end = segment_j_start + 1; segment_j_end <= size_j - 1; ++segment_j_end)
              {
                size_t length_j = segment_j_end - segment_j_start;
                if (CROSS_LENGTH_LIMIT != -1 && length_j > CROSS_LENGTH_LIMIT)
                  break;

                auto cross_operator = std::make_unique<Cross>();
                cross_operator->vehicle_i = vehicle_i;
                cross_operator->vehicle_j = vehicle_j;
                cross_operator->segment_i_start = segment_i_start;
                cross_operator->segment_j_start = segment_j_start;
                cross_operator->segment_i_end = segment_i_end;
                cross_operator->segment_j_end = segment_j_end;
                result_vector.push_back(std::move(cross_operator));
              }
            }
          }
        }
      }
    }
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