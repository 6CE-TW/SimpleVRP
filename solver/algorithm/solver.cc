#include <iostream>
#include <iomanip>
#include <future>
#include "solver/algorithm/solver.h"
#include "solver/algorithm/initial_solution/initial_solution.h"
#include "solver/algorithm/local_search/local_search.h"
#include "solver/algorithm/parameter/parameter.h"
#include "solver/algorithm/config.h"

// TODO: use glog to print log
// #include <glog/logging.h>

void SimpleVRPSolver::SetUsableLocalSearch(std::unordered_map<LocalSearchEnum, bool> local_search_list)
{
  this->local_search_parameter.set_usable_local_search(local_search_list);
}

void SimpleVRPSolver::Solve()
{
  this->RecordSolverStartTime();
  this->GetInitialSolution();
  this->EncodeRouteToNodeRecord();

  if (LOG)
  {
    std::cout << "cost: " << std::fixed << std::setprecision(2) << this->_cost
              << " Initial Solution - " << InitialSolutionStrategyEnumToString(this->initial_solution_strategy) << "\n";
  }

  auto modified_local_search_list = this->local_search_parameter.usable_local_search();

  // this->PerformLocalSearchOnce();
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

Solution SimpleVRPSolver::ExtractSolution()
{
  Solution solution = Solution();

  for (std::size_t i = 0; i < this->_num_of_vehicles; ++i)
  {
    Vehicle vehicle = this->vehicle(i);
    VehicleTaskRoute vehicle_task_route(vehicle);

    std::size_t route_length = node_records.at(i).size();

    for (std::size_t j = 0; j < route_length; ++j)
    {
      std::size_t node = node_records.at(i).at(j);
      Task task(j, node, this->_parameter.destinations.at(node));
      
      if(j > 0)
      {
        std::size_t prev_node = vehicle_task_route.tasks.back().index;
        task.transit_distance = this->_distance_matrix[prev_node][node];
        task.transit_time = this->_duration_matrix[prev_node][node];
        task.transit_cost = this->_cost_matrix[prev_node][node];
      }

      vehicle_task_route.tasks.push_back(task);
    }

    solution.vehicle_task_routes.push_back(vehicle_task_route);
  }

  return solution;
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

void SimpleVRPSolver::PerformLocalSearchOnce()
{
  LocalSearchGenerator local_search_generator(this->node_records, this->_num_of_vehicles, this->local_search_parameter.usable_local_search());
  std::vector<std::unique_ptr<LocalSearch>> local_search_list = local_search_generator.GenerateLocalSearchList();

  if (DEBUG)
  {
    VerifyLocalSearchList(local_search_list);
  }

  auto [best_cost, best_op] = FindBestLocalSearch(local_search_list, this->node_records, this->_cost_matrix);

  if (best_op && this->_cost > best_cost)
  {
    if (DEBUG)
    {
      std::cout << "=== Best Operation ===\n";
      best_op->Print();
      std::cout << "cost: " << this->_cost << " -> " << best_cost << "\n";
    }
    else if (LOG)
    {
      std::cout << "cost: " << std::fixed << std::setprecision(2) << best_cost << " ";
      best_op->Print();
    }

    this->node_records = std::move(best_op->test_solution);
    this->_cost = best_cost;
  }
  else
  {
    if (DEBUG)
    {
      std::cout << "No improvement found.\n";
    }
  }
}

void SimpleVRPSolver::PerformLocalSearchGreedy()
{
  while (true)
  {
    LocalSearchGenerator local_search_generator(this->node_records, this->_num_of_vehicles, this->local_search_parameter.usable_local_search());
    std::vector<std::unique_ptr<LocalSearch>> local_search_list = local_search_generator.GenerateLocalSearchList();

    if (DEBUG)
    {
      VerifyLocalSearchList(local_search_list);
    }

    auto [best_cost, best_op] = FindBestLocalSearch(local_search_list, this->node_records, this->_cost_matrix);

    if (best_op && this->_cost > best_cost)
    {
      if (DEBUG)
      {
        std::cout << "=== Best Operation ===\n";
        best_op->Print();
        std::cout << "cost: " << this->_cost << " -> " << best_cost << "\n";
      }
      else if (LOG)
      {
        std::cout << "cost: " << std::fixed << std::setprecision(2) << best_cost << " ";
        best_op->Print();
      }

      this->node_records = std::move(best_op->test_solution);
      this->_cost = best_cost;
    }
    else
    {
      if (DEBUG)
      {
        std::cout << "No improvement found.\n";
      }

      break;
    }
  }
}

void SimpleVRPSolver::PerformLocalSearchGuidedLocalSearch(std::atomic<bool> &timeout_flag)
{
  std::vector<std::vector<int>> penalty_counter_matrix(this->_num_of_nodes, std::vector<int>(this->_num_of_nodes, 0));

  while (timeout_flag == false)
  {
    LocalSearchGenerator local_search_generator(this->node_records, this->_num_of_vehicles, this->local_search_parameter.usable_local_search());
    std::vector<std::unique_ptr<LocalSearch>> local_search_list = local_search_generator.GenerateLocalSearchList();

    if (DEBUG)
    {
      VerifyLocalSearchList(local_search_list);
    }

    auto [best_cost, best_augmented_cost, best_op] = FindBestLocalSearchGuidedLocalSearch(
        local_search_list, this->node_records, this->_cost_matrix, penalty_counter_matrix);

    if (best_op && this->_cost > best_cost)
    {
      if (DEBUG)
      {
        std::cout << "=== Best Operation ===\n";
        best_op->Print();
        std::cout << "cost: " << this->_cost << " -> " << best_cost << "\n";
      }
      else if (LOG)
      {
        std::cout << "cost: " << std::fixed << std::setprecision(2) << best_cost
                  << " augmented_cost: " << best_augmented_cost
                  << " time: " << now_unixtime_milliseconds() - this->solve_start_unixtime << " / " << TIME_LIMIT * 1000
                  << " ";
        best_op->Print();
      }

      this->node_records = std::move(best_op->test_solution);
      this->_cost = best_cost;
    }
    else
    {
      if (DEBUG)
      {
        std::cout << "=== Best Operation ===\n";
        best_op->Print();
        std::cout << "cost: " << this->_cost << " -> " << best_cost << "\n";
      }
      else if (LOG)
      {
        std::cout << "cost: " << std::fixed << std::setprecision(2) << best_cost
                  << " augmented_cost: " << best_augmented_cost
                  << " time: " << now_unixtime_milliseconds() - this->solve_start_unixtime << " / " << TIME_LIMIT * 1000
                  << " ";
        best_op->Print();
      }

      this->node_records = std::move(best_op->test_solution);
      double max_utility = 0;
      std::pair<std::size_t, std::size_t> max_utility_edge;
      for (std::size_t i = 0; i < this->_num_of_vehicles; ++i)
      {
        const std::vector<std::size_t> route = this->node_records.at(i);
        for (std::size_t j = 1; j < route.size(); ++j)
        {
          double temp_utility = this->_cost_matrix[route[j - 1]][route[j]] / (1 + penalty_counter_matrix[route[j - 1]][route[j]]);
          if (temp_utility > max_utility)
          {
            max_utility = temp_utility;
            max_utility_edge = {route[j - 1], route[j]};
          }
        }
      }

      penalty_counter_matrix[max_utility_edge.first][max_utility_edge.second] += 1;
    }
  }
}

void SimpleVRPSolver::PerformLocalSearch()
{
  if (this->metaheuristic_strategy == MetaheuristicStrategy::DO_NOTHING)
  {
  }
  if (this->metaheuristic_strategy == MetaheuristicStrategy::GREEDY)
  {
    this->PerformLocalSearchGreedy();
  }
  if (this->metaheuristic_strategy == MetaheuristicStrategy::EPSILON_GREEDY)
  {
  }
  if (this->metaheuristic_strategy == MetaheuristicStrategy::GUIDED_LOCAL_SEARCH)
  {
    std::atomic<bool> timeout_flag(false);
    // use std::async to async run PerformLocalSearchGuidedLocalSearch
    auto future = std::async(
        std::launch::async,
        [this, &timeout_flag]()
        {this->PerformLocalSearchGuidedLocalSearch(timeout_flag); 
          return; });

    // Wait for result or timeout
    if (future.wait_for(std::chrono::seconds(TIME_LIMIT)) == std::future_status::ready)
    {
      // not out of time
    }
    else
    {
      timeout_flag = true;
    }
  }
  if (this->metaheuristic_strategy == MetaheuristicStrategy::TABU_SEARCH)
  {
  }
  if (this->metaheuristic_strategy == MetaheuristicStrategy::SIMULATED_ANNEALING_SEARCH)
  {
  }
}