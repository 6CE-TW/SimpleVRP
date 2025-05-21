#include "algorithm/parameter/parameter.h"

LocalSearchParameter::LocalSearchParameter()
{
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