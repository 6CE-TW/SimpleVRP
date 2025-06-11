#include <set>

#include "solver/algorithm/solver.h"
#include "solver/algorithm/initial_solution/initial_solution.h"

std::string InitialSolutionStrategyEnumToString(InitialSolutionStrategy initial_solution_strategy)
{
  if (initial_solution_strategy == InitialSolutionStrategy::CHEAPEST_NEIGHBOR)
  {
    return "CHEAPEST_NEIGHBOR";
  }
  if (initial_solution_strategy == InitialSolutionStrategy::CHEAPEST_NEIGHBOR_MULTIPLE_VEHICLE)
  {
    return "CHEAPEST_NEIGHBOR_MULTIPLE_VEHICLE";
  }
  return "";
}

void SimpleVRPSolver::GetInitialSolution()
{
  if (this->initial_solution_strategy == InitialSolutionStrategy::CHEAPEST_NEIGHBOR)
  {
    this->InitialSolutionCheapestNeighbor();
  }
  if (this->initial_solution_strategy == InitialSolutionStrategy::CHEAPEST_NEIGHBOR_MULTIPLE_VEHICLE)
  {
    this->InitialSolutionCheapestNeighborMultipleVehicle();
  }
  if (this->initial_solution_strategy == InitialSolutionStrategy::GLOBAL_MINIMAL)
  {
    this->InitialSolutionGlobalMinimal();
  }
}

void SimpleVRPSolver::InitialSolutionCheapestNeighbor()
{
  std::set<std::size_t> traversed_nodes;
  std::size_t latest_arrival_node = this->_start_node_indices[0];

  for (std::size_t i = 0; i < this->_num_of_middle_nodes; ++i)
  {
    std::size_t best_idx = 0;
    std::size_t minimal_cost = INT32_MAX;
    for (std::size_t j = 0; j < this->_num_of_nodes; ++j)
    {
      if (this->_is_middle_node[j] == false ||
          traversed_nodes.find(j) != traversed_nodes.end())
      {
        continue;
      }

      if (this->_cost_matrix[latest_arrival_node][j] < minimal_cost)
      {
        best_idx = j;
        minimal_cost = _cost_matrix[latest_arrival_node][j];
      }
    }

    Route temp_route{latest_arrival_node, best_idx, 0};
    this->route_records[0].push_back(temp_route);
    this->_cost += minimal_cost;

    traversed_nodes.insert(best_idx);
    latest_arrival_node = best_idx;
  }
  Route temp_route{latest_arrival_node, this->_num_of_nodes - 1, 0};
  this->route_records[0].push_back(temp_route);
  this->_cost += this->_cost_matrix[latest_arrival_node][this->_num_of_nodes - 1];

  for (std::size_t i = 1; i < this->_num_of_vehicles; ++i)
  {
    Route temp_route{this->_start_node_indices[i], this->_end_node_indices[i], i};
    this->route_records[i].push_back(temp_route);
    this->_cost += this->_cost_matrix[this->_start_node_indices[i]][this->_end_node_indices[i]];
  }
}

void SimpleVRPSolver::InitialSolutionCheapestNeighborMultipleVehicle()
{
  std::set<std::size_t> traversed_nodes;
  std::vector<std::size_t> latest_arrival_node = this->_start_node_indices;

  for (std::size_t i = 0; i < this->_num_of_middle_nodes; ++i)
  {
    std::size_t best_idx = 0;
    std::size_t best_vehicle = 0;
    std::size_t minimal_cost = INT32_MAX;

    for (std::size_t j = 0; j < this->_num_of_nodes; ++j)
    {
      if (this->_is_middle_node[j] == false ||
          traversed_nodes.find(j) != traversed_nodes.end())
      {
        continue;
      }

      for (std::size_t k = 0; k < this->_num_of_vehicles; ++k)
      {
        if (this->_cost_matrix[latest_arrival_node[k]][j] < minimal_cost)
        {
          best_idx = j;
          best_vehicle = k;
          minimal_cost = _cost_matrix[latest_arrival_node[k]][j];
        }
      }
    }

    Route temp_route{latest_arrival_node[best_vehicle], best_idx, best_vehicle};
    this->route_records[best_vehicle].push_back(temp_route);
    this->_cost += minimal_cost;

    traversed_nodes.insert(best_idx);
    latest_arrival_node[best_vehicle] = best_idx;
  }

  for (std::size_t i = 0; i < this->_num_of_vehicles; ++i)
  {
    Route temp_route{latest_arrival_node[i], this->_end_node_indices[i], i};
    this->route_records[i].push_back(temp_route);
    this->_cost += this->_cost_matrix[latest_arrival_node[i]][this->_end_node_indices[i]];
  }
}

void SimpleVRPSolver::InitialSolutionGlobalMinimal()
{
  std::set<std::size_t> traversed_nodes;

  // Establish all edges and sort them
  std::vector<Route> edges;
  for (std::size_t i = 0; i < this->_num_of_nodes; ++i)
  {
    for (std::size_t j = 0; j < this->_num_of_nodes; ++j)
    {
      if (i != j)
      {
        Route temp_route{i, j, this->_num_of_vehicles, this->_cost_matrix[i][j]};
        edges.push_back(temp_route);
      }
    }
  }
  std::sort(edges.begin(), edges.end(), [](const Route &a, const Route &b)
            { return a.cost < b.cost; });

  // initialize m routes
  std::vector<std::pair<std::size_t, std::size_t>> subroute_start_end_nodes;
  std::vector<PathRoutes> path_routes;
  std::vector<std::size_t> enter_node_quota(this->_num_of_nodes, 0);
  std::vector<std::size_t> leave_node_quota(this->_num_of_nodes, 0);
  for (std::size_t i = 0; i < this->_num_of_vehicles; ++i)
  {
    subroute_start_end_nodes.push_back({this->_num_of_nodes, this->_start_node_indices[i]});
    subroute_start_end_nodes.push_back({this->_end_node_indices[i], this->_num_of_nodes});

    Route dummy_start_route{this->_num_of_nodes, this->_start_node_indices[i], i, 0};
    Route dummy_end_route{this->_end_node_indices[i], this->_num_of_nodes, i, 0};

    PathRoutes start_path{{dummy_start_route}, i};
    PathRoutes end_path{{dummy_end_route}, i};
    path_routes.push_back(start_path);
    path_routes.push_back(end_path);

    leave_node_quota[this->_start_node_indices[i]] += 1;
    enter_node_quota[this->_end_node_indices[i]] += 1;
  }
  for (std::size_t i = 0; i < this->_num_of_nodes; ++i)
  {
    if (this->_is_middle_node[i])
    {
      enter_node_quota[i] = 1;
      leave_node_quota[i] = 1;
    }
  }

  std::vector<PathRoutes> complete_path_routes;
  std::size_t num_complete_path_routes = 0;
  for (const Route &e : edges)
  {
    if (leave_node_quota[e.prev] == 0 || enter_node_quota[e.next] == 0)
    {
      continue;
    }

    bool occur_sub_cycle = false;
    int prev_route_index = -1;
    int next_route_index = -1;
    for (std::size_t i = 0; i < subroute_start_end_nodes.size(); ++i)
    {
      std::pair<std::size_t, std::size_t> pair = subroute_start_end_nodes.at(i);
      if (e.prev == pair.second && e.next == pair.first)
      {
        occur_sub_cycle = true;
        break;
      }

      if (e.prev == pair.second)
      {
        prev_route_index = i;
      }
      else if (e.next == pair.first)
      {
        next_route_index = i;
      }
    }
    if (occur_sub_cycle)
    {
      continue;
    }

    // create a new path which is not connected to any other
    if (prev_route_index == -1 && next_route_index == -1)
    {
      subroute_start_end_nodes.push_back({e.prev, e.next});
      PathRoutes middle_path{{e}, this->_num_of_vehicles};
      path_routes.push_back(middle_path);

      leave_node_quota[e.prev] -= 1;
      enter_node_quota[e.next] -= 1;

      this->_cost += e.cost;
    }
    // connect two path through edge e
    else if (prev_route_index != -1 && next_route_index != -1)
    {
      if (path_routes[prev_route_index].vehicle < this->_num_of_vehicles &&
          path_routes[next_route_index].vehicle < this->_num_of_vehicles)
      {
        if (path_routes[prev_route_index].vehicle != path_routes[next_route_index].vehicle)
        {
          continue;
        }
        else // path_routes[prev_route_index].vehicle == path_routes[next_route_index].vehicle
        {
          // prev path and next path is belong to the same vehicle
          // thus, connect two path by current edge and the route is complete

          bool has_left_unconnected_node = false;
          for (std::size_t i = 0; i < this->_num_of_nodes; ++i)
          {
            if (i == e.prev)
            {
              if (leave_node_quota[i] > 1)
              {
                has_left_unconnected_node = true;
                break;
              }
            }
            else if (i == e.next)
            {
              if (enter_node_quota[i] > 1)
              {
                has_left_unconnected_node = true;
                break;
              }
            }
            else
            {
              if (leave_node_quota[i] > 0 || enter_node_quota[i] > 0)
              {
                has_left_unconnected_node = true;
                break;
              }
            }
          }

          if (has_left_unconnected_node == true && num_complete_path_routes == this->_num_of_vehicles - 1)
          {
            continue;
          }

          std::vector<Route> complete_routes = path_routes[prev_route_index].routes;
          complete_routes.push_back(e);
          for (const auto &r : path_routes[next_route_index].routes)
          {
            complete_routes.push_back(r);
          }

          PathRoutes finished_route{complete_routes, path_routes[prev_route_index].vehicle};
          complete_path_routes.push_back(finished_route);
          num_complete_path_routes += 1;

          path_routes.erase(path_routes.begin() + std::max(prev_route_index, next_route_index));
          path_routes.erase(path_routes.begin() + std::min(prev_route_index, next_route_index));
          subroute_start_end_nodes.erase(subroute_start_end_nodes.begin() + std::max(prev_route_index, next_route_index));
          subroute_start_end_nodes.erase(subroute_start_end_nodes.begin() + std::min(prev_route_index, next_route_index));
          leave_node_quota[e.prev] -= 1;
          enter_node_quota[e.next] -= 1;

          this->_cost += e.cost;

          if (num_complete_path_routes == this->_num_of_vehicles)
          {
            break;
          }
        }
      }
      else if (path_routes[prev_route_index].vehicle == this->_num_of_vehicles &&
               path_routes[next_route_index].vehicle == this->_num_of_vehicles)
      {
        // connect two paths that have not yet belonged to any vehicle

        std::vector<Route> connected_routes = path_routes[prev_route_index].routes;
        connected_routes.push_back(e);
        for (const auto &r : path_routes[next_route_index].routes)
        {
          connected_routes.push_back(r);
        }

        PathRoutes connected_path{connected_routes, path_routes[prev_route_index].vehicle};
        path_routes.push_back(connected_path);
        subroute_start_end_nodes.push_back({connected_routes.front().prev, connected_routes.back().next});

        path_routes.erase(path_routes.begin() + std::max(prev_route_index, next_route_index));
        path_routes.erase(path_routes.begin() + std::min(prev_route_index, next_route_index));
        subroute_start_end_nodes.erase(subroute_start_end_nodes.begin() + std::max(prev_route_index, next_route_index));
        subroute_start_end_nodes.erase(subroute_start_end_nodes.begin() + std::min(prev_route_index, next_route_index));
        leave_node_quota[e.prev] -= 1;
        enter_node_quota[e.next] -= 1;

        this->_cost += e.cost;
      }
      else if (path_routes[next_route_index].vehicle == this->_num_of_vehicles)
      {
        // connect one path that has not yet belonged to any vehicle to a start path

        std::vector<Route> connected_routes = path_routes[prev_route_index].routes;
        connected_routes.push_back(e);
        for (const auto &r : path_routes[next_route_index].routes)
        {
          connected_routes.push_back(r);
        }

        PathRoutes connected_path{connected_routes, path_routes[prev_route_index].vehicle};
        path_routes.push_back(connected_path);
        subroute_start_end_nodes.push_back({connected_routes.front().prev, connected_routes.back().next});

        path_routes.erase(path_routes.begin() + std::max(prev_route_index, next_route_index));
        path_routes.erase(path_routes.begin() + std::min(prev_route_index, next_route_index));
        subroute_start_end_nodes.erase(subroute_start_end_nodes.begin() + std::max(prev_route_index, next_route_index));
        subroute_start_end_nodes.erase(subroute_start_end_nodes.begin() + std::min(prev_route_index, next_route_index));
        leave_node_quota[e.prev] -= 1;
        enter_node_quota[e.next] -= 1;

        this->_cost += e.cost;
      }
      else if (path_routes[prev_route_index].vehicle == this->_num_of_vehicles)
      {
        // connect one path that has not yet belonged to any vehicle to an end path

        std::vector<Route> connected_routes = path_routes[prev_route_index].routes;
        connected_routes.push_back(e);
        for (const auto &r : path_routes[next_route_index].routes)
        {
          connected_routes.push_back(r);
        }

        PathRoutes connected_path{connected_routes, path_routes[next_route_index].vehicle};
        path_routes.push_back(connected_path);
        subroute_start_end_nodes.push_back({connected_routes.front().prev, connected_routes.back().next});

        path_routes.erase(path_routes.begin() + std::max(prev_route_index, next_route_index));
        path_routes.erase(path_routes.begin() + std::min(prev_route_index, next_route_index));
        subroute_start_end_nodes.erase(subroute_start_end_nodes.begin() + std::max(prev_route_index, next_route_index));
        subroute_start_end_nodes.erase(subroute_start_end_nodes.begin() + std::min(prev_route_index, next_route_index));
        leave_node_quota[e.prev] -= 1;
        enter_node_quota[e.next] -= 1;

        this->_cost += e.cost;
      }
    }
    // connect edge e to a start path
    else if (prev_route_index != -1)
    {
      path_routes[prev_route_index].routes.push_back(e);
      subroute_start_end_nodes[prev_route_index].second = e.next;

      leave_node_quota[e.prev] -= 1;
      enter_node_quota[e.next] -= 1;

      this->_cost += e.cost;
    }
    // connect edge e to an end path
    else if (next_route_index != -1)
    {
      path_routes[next_route_index].routes.insert(path_routes[next_route_index].routes.begin(), e);
      subroute_start_end_nodes[next_route_index].first = e.prev;

      leave_node_quota[e.prev] -= 1;
      enter_node_quota[e.next] -= 1;

      this->_cost += e.cost;
    }
  }

  this->route_records = std::vector<std::vector<Route>>(this->_num_of_vehicles);
  for (PathRoutes &path : complete_path_routes)
  {
    path.routes.erase(path.routes.begin());
    path.routes.pop_back();
    this->route_records[path.vehicle] = path.routes;
  }
}