#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include <iostream>
#include <vector>
#include <unordered_map>

void SwapSubsegments(
    std::vector<std::size_t> &a, size_t a_start, size_t a_end,
    std::vector<std::size_t> &b, size_t b_start, size_t b_end);

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
  EXCHANGE = 11,
  CROSS = 12,

  LAMBDA_EXCHANGE = 21,

  LNS = 30,
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

class RelocateSameVehicle : public LocalSearch
{
public:
  size_t vehicle;
  size_t original_path_position;
  size_t new_path_position;

  void Print() const override
  {
    std::cout << "Relocate - Vehicle: " << this->vehicle
              << " Position [" << this->original_path_position << "] -> [" << this->new_path_position << "]\n";
  }

  void Apply(const std::vector<std::vector<std::size_t>> &input,
             std::vector<std::vector<std::size_t>> &output) const override
  {
    output = input;
    if (vehicle >= output.size())
      return;
    auto &route = output[vehicle];

    if (original_path_position >= route.size())
      return;

    std::size_t node = route.at(original_path_position);

    std::size_t adjusted_new_pos = new_path_position;
    if (original_path_position < new_path_position)
      adjusted_new_pos -= 1;

    if (adjusted_new_pos > route.size())
      return;

    route.erase(route.begin() + original_path_position);
    route.insert(route.begin() + adjusted_new_pos, node);
  }
};

class RelocateDiffVehicle : public LocalSearch
{
public:
  size_t original_vehicle;
  size_t original_path_position;
  size_t new_vehicle;
  size_t new_path_position;

  void Print() const override
  {
    std::cout << "Relocate - Path Position: [" << this->original_vehicle << "][" << this->original_path_position
              << "] -> Path Position: [" << this->new_vehicle << "][" << this->new_path_position << "]\n";
  }

  void Apply(const std::vector<std::vector<std::size_t>> &input,
             std::vector<std::vector<std::size_t>> &output) const override
  {
    output = input;
    if (original_vehicle >= output.size() || new_vehicle >= output.size())
      return;
    auto &original_route = output[original_vehicle];
    auto &new_route = output[new_vehicle];

    if (original_path_position >= original_route.size() || new_path_position >= new_route.size())
      return;

    std::size_t node = original_route.at(original_path_position);
    original_route.erase(original_route.begin() + original_path_position);
    new_route.insert(new_route.begin() + new_path_position, node);
  }
};

class Exchange : public LocalSearch
{
public:
  size_t vehicle_i;
  size_t path_position_i;
  size_t vehicle_j;
  size_t path_position_j;

  void Print() const override
  {
    std::cout << "Exchange - Path Position: [" << this->vehicle_i << "][" << this->path_position_i
              << "] <-> Path Position: [" << this->vehicle_j << "][" << this->path_position_j << "]\n";
  }

  void Apply(const std::vector<std::vector<std::size_t>> &input,
             std::vector<std::vector<std::size_t>> &output) const override
  {
    output = input;
    if (vehicle_i >= output.size() || vehicle_j >= output.size())
      return;
    auto &route_i = output[vehicle_i];
    auto &route_j = output[vehicle_j];

    if (path_position_i >= route_i.size() || path_position_j >= route_j.size())
      return;

    std::swap(route_i[path_position_i], route_j[path_position_j]);
  }
};

class Cross : public LocalSearch
{
public:
  size_t vehicle_i;
  size_t segment_i_start;
  size_t segment_i_end;
  size_t vehicle_j;
  size_t segment_j_start;
  size_t segment_j_end;

  void Print() const override
  {
    std::cout << "Cross - Path Position: [" << this->vehicle_i << "][" << this->segment_i_start << ", " << this->segment_i_end
              << ") <-> Path Position: [" << this->vehicle_j << "][" << this->segment_j_start << ", " << this->segment_j_end << ")\n";
  }

  void Apply(const std::vector<std::vector<std::size_t>> &input,
             std::vector<std::vector<std::size_t>> &output) const override
  {
    output = input;
    if (vehicle_i >= output.size() || vehicle_j >= output.size())
      return;
    auto &route_i = output[vehicle_i];
    auto &route_j = output[vehicle_j];

    SwapSubsegments(route_i, this->segment_i_start, this->segment_i_end,
                    route_j, this->segment_j_start, this->segment_j_end);
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
      {LocalSearchEnum::EXCHANGE, true},
      {LocalSearchEnum::CROSS, true},
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