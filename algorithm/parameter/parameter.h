#ifndef PARAMETER_H
#define PARAMETER_H

#include <unordered_map>
#include "algorithm/local_search/local_search.h"

class LocalSearchParameter
{
private:
  std::unordered_map<LocalSearchEnum, bool> _rearranged_local_search = {
      {LocalSearchEnum::TWO_OPT, true},
      {LocalSearchEnum::OR_OPT, true},
      {LocalSearchEnum::THREE_OPT, true},
      {LocalSearchEnum::RELOCATE_SAME_VEHICLE, true},
      {LocalSearchEnum::RELOCATE_DIFF_VEHICLE, true},
      {LocalSearchEnum::EXCHANGE, true},
      {LocalSearchEnum::CROSS, true},
      {LocalSearchEnum::LNS, true},
  };

public:
  LocalSearchParameter() {};
  LocalSearchParameter(std::unordered_map<LocalSearchEnum, bool> usable_local_search);

  std::unordered_map<LocalSearchEnum, bool> rearranged_local_search()
  {
    return this->_rearranged_local_search;
  }

  void set_rearranged_local_search(const std::unordered_map<LocalSearchEnum, bool> usable_local_search)
  {
    this->_rearranged_local_search = usable_local_search;
    if (this->_rearranged_local_search[LocalSearchEnum::THREE_OPT])
    {
      this->_rearranged_local_search[LocalSearchEnum::TWO_OPT] = true;
      // Here we assume OR_OPT_LENGTH_LIMIT_IN_THREE_OPT >= OR_OPT_LENGTH_LIMIT
      // TODO: Consider the situation that length limit in OR OPT is larger
      this->_rearranged_local_search[LocalSearchEnum::OR_OPT] = false;
      this->_rearranged_local_search[LocalSearchEnum::RELOCATE_SAME_VEHICLE] = false;
    }
    if (this->_rearranged_local_search[LocalSearchEnum::OR_OPT])
    {
      this->_rearranged_local_search[LocalSearchEnum::RELOCATE_SAME_VEHICLE] = false;
    }
    if (this->_rearranged_local_search[LocalSearchEnum::CROSS])
    {
      this->_rearranged_local_search[LocalSearchEnum::EXCHANGE] = false;
    }
  }
};

LocalSearchParameter::LocalSearchParameter(std::unordered_map<LocalSearchEnum, bool> usable_local_search)
{
  this->_rearranged_local_search = usable_local_search;
  if (this->_rearranged_local_search[LocalSearchEnum::THREE_OPT])
  {
    this->_rearranged_local_search[LocalSearchEnum::TWO_OPT] = true;
    // Here we assume OR_OPT_LENGTH_LIMIT_IN_THREE_OPT >= OR_OPT_LENGTH_LIMIT
    // TODO: Consider the situation that length limit in OR OPT is larger
    this->_rearranged_local_search[LocalSearchEnum::OR_OPT] = false;
    this->_rearranged_local_search[LocalSearchEnum::RELOCATE_SAME_VEHICLE] = false;
  }
  if (this->_rearranged_local_search[LocalSearchEnum::OR_OPT])
  {
    this->_rearranged_local_search[LocalSearchEnum::RELOCATE_SAME_VEHICLE] = false;
  }
  if (this->_rearranged_local_search[LocalSearchEnum::CROSS])
  {
    this->_rearranged_local_search[LocalSearchEnum::EXCHANGE] = false;
  }
}

#endif // PARAMETER_H