#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include <iostream>
#include <vector>
#include <unordered_map>

enum LocalSearchEnum
{
  // intra route operation
  TWO_OPT = 0,
  OR_OPT = 1,
  SWAP = 2,
  THREE_OPT = 3,
  RELOCATE_SAME_VEHICLE = 4,

  // inter route operation
  RELOCATE_DIFF_VEHICLE = 10,

  CROSS_EXCHANGE = 5,
  LAMBDA_EXCHANGE = 6,

  LNS = 7,
};

class LocalSearch
{
public:
  virtual ~LocalSearch() = default;
  virtual void Print() const = 0;

  virtual void Apply(
      const std::vector<std::vector<std::size_t>> &input,
      std::vector<std::vector<std::size_t>> &output) const = 0;

  std::vector<std::vector<std::size_t>> test_solution;
};

class TwoOpt : public LocalSearch
{
public:
  size_t path_start;
  size_t path_end;
  size_t vehicle;

  void Print() const override
  {
    std::cout << "TwoOpt - Vehicle: " << this->vehicle
              << " Reverse Path: (" << this->path_start << " -> " << this->path_end << ")\n";
  }

  void Apply(const std::vector<std::vector<std::size_t>> &input,
             std::vector<std::vector<std::size_t>> &output) const override
  {
    output = input;
    if (vehicle >= output.size())
      return;
    auto &route = output[vehicle];
    if (path_start < path_end && path_end < route.size())
    {
      std::reverse(route.begin() + path_start, route.begin() + path_end + 1);
    }
  }
};

class LocalSearchGenerator
{
private:
  std::unordered_map<LocalSearchEnum, bool> usable_local_search = {
      {LocalSearchEnum::TWO_OPT, true},
      {LocalSearchEnum::OR_OPT, true},
      {LocalSearchEnum::SWAP, true},
      {LocalSearchEnum::THREE_OPT, true},
      {LocalSearchEnum::RELOCATE_SAME_VEHICLE, true},
      {LocalSearchEnum::RELOCATE_DIFF_VEHICLE, true},
      {LocalSearchEnum::CROSS_EXCHANGE, true},
      {LocalSearchEnum::LAMBDA_EXCHANGE, true},
      {LocalSearchEnum::LNS, true},
  };
  std::vector<std::vector<std::size_t>> _node_records;
  std::size_t _num_of_vehicle;

public:
  LocalSearchGenerator(std::vector<std::vector<std::size_t>> node_records, std::size_t num_of_vehicle)
  {
    this->_node_records = node_records;
    this->_num_of_vehicle = num_of_vehicle;
  }
  std::vector<std::unique_ptr<LocalSearch>> GenerateLocalSearchList();
};

void VerifyLocalSearchList(const std::vector<std::unique_ptr<LocalSearch>> &local_search_list);

std::pair<double, const LocalSearch *> FindBestLocalSearch(
    const std::vector<std::unique_ptr<LocalSearch>> &local_search_list,
    const std::vector<std::vector<std::size_t>> &original_node_records,
    const std::vector<std::vector<double>> &cost_matrix);

#endif // LOCAL_SEARCH_H