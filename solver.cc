#include <iostream>
#include "solver.h"
#include "initial_solution/initial_solution.h"

void SimpleVRPSolver::Solve()
{
  this->GetInitialSolution();
}

void SimpleVRPSolver::PrintSolution()
{
  for (std::size_t i = 0; i < this->_num_of_vehicles; ++i)
  {
    std::size_t route_length = route_records.at(i).size();

    std::cout << "vehicle " << i << "\n";
    for (std::size_t j = 0; j < route_length; ++j)
    {
      const Route route = route_records.at(i).at(j);
      std::cout << "(" << route.prev << ", " << route.next << ") ";
    }
    std::cout << "\n\n";
  }
  std::cout << "cost: " << this->_cost << "\n";
}