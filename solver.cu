#include "solver.h"

class SimpleVRPSolver
{
private:
  std::vector<std::vector<double>> _cost_matrix;

public:
  SimpleVRPSolver(std::vector<std::vector<double>> cost_matrix)
  {
    this->_cost_matrix = cost_matrix;
  }
};