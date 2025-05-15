#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include <iostream>
#include <vector>

enum LocalSearchEnum
{
  TWO_OPT = 0,
  OR_OPT = 1,
  RELOCATE = 2,
  SWAP = 3,

  CROSS_EXCHANGE = 4,
  THREE_OPT = 5,
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

  size_t vehicle;
  std::vector<std::vector<std::size_t>> test_solution;
};

class TwoOpt : public LocalSearch
{
public:
  size_t path_start;
  size_t path_end;

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
  std::vector<bool> usable_local_search = {
      true, // 0: TWO_OPT
      true, // 1: OR_OPT
      true, // 2: RELOCATE
      true, // 3: SWAP
      true, // 4: CROSS_EXCHANGE
      true, // 5: THREE_OPT
      true, // 6: LAMBDA_EXCHANGE
      true, // 7: LNS
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