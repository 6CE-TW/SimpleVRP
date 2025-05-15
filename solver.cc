#include <iostream>
#include "solver.h"
#include "initial_solution/initial_solution.h"
#include "local_search/local_search.h"

void SimpleVRPSolver::Solve()
{
  this->GetInitialSolution();
  this->EncodeRouteToNodeRecord();
  this->PerformLocalSearch();
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

void SimpleVRPSolver::PrintNodeRecords()
{
  for (std::size_t i = 0; i < this->_num_of_vehicles; ++i)
  {
    std::size_t route_length = node_records.at(i).size();

    std::cout << "vehicle " << i << "\n";
    std::cout << node_records.at(i).at(0);
    for (std::size_t j = 1; j < route_length; ++j)
    {
      std::cout << " -> " << node_records.at(i).at(j);
    }
    std::cout << "\n\n";
  }
  std::cout << "cost: " << this->_cost << "\n";
}

void SimpleVRPSolver::EncodeRouteToNodeRecord()
{
  std::vector<std::vector<std::size_t>> node_records;
  for (std::size_t i = 0; i < this->_num_of_vehicles; ++i)
  {
    std::size_t route_length = route_records.at(i).size();

    std::vector<std::size_t> single_vehicle_node_records = {0};
    for (std::size_t j = 0; j < route_length; ++j)
    {
      const Route route = route_records.at(i).at(j);
      single_vehicle_node_records.push_back(route.next);
    }
    node_records.push_back(single_vehicle_node_records);
  }
  this->node_records = node_records;
}

void SimpleVRPSolver::PerformLocalSearch()
{
  LocalSearchGenerator local_search_generator(this->node_records, this->_num_of_vehicles);
  std::vector<std::unique_ptr<LocalSearch>> local_search_list = local_search_generator.GenerateLocalSearchList();
  VerifyLocalSearchList(local_search_list);

  auto [best_cost, best_op] = FindBestLocalSearch(local_search_list, this->node_records, this->_cost_matrix);

  if (best_op)
  {
    std::cout << "=== Best Operation ===\n";
    best_op->Print();

    this->node_records = std::move(best_op->test_solution);
    this->_cost = best_cost;
    // best_op->ApplyInPlace(this->node_records);
  }
  else
  {
    std::cout << "No improvement found.\n";
  }
}