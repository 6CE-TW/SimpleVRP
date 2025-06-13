#include <set>

#include "solver/algorithm/solver.h"
#include "solver/algorithm/initial_solution/initial_solution.h"

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
  if (this->initial_solution_strategy == InitialSolutionStrategy::GLOBAL_MAXIMUM_VARIANCE_FIRST)
  {
    this->InitialSolutionGlobalMaximumVarianceFirst();
  }

  this->_initial_cost = this->_cost;
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
  Route temp_route{latest_arrival_node, this->_end_node_indices[0], 0};
  this->route_records[0].push_back(temp_route);
  this->_cost += this->_cost_matrix[latest_arrival_node][this->_end_node_indices[0]];

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
  struct PathUnit
  {
    std::pair<std::size_t, std::size_t> endpoints;
    PathRoutes path_route;
  };
  std::vector<PathUnit> path_units;

  // std::vector<std::pair<std::size_t, std::size_t>> subroute_start_end_nodes;
  // std::vector<PathRoutes> path_routes;
  std::vector<std::size_t> enter_node_quota(this->_num_of_nodes, 0);
  std::vector<std::size_t> leave_node_quota(this->_num_of_nodes, 0);
  for (std::size_t i = 0; i < this->_num_of_vehicles; ++i)
  {
    Route dummy_start_route{this->_num_of_nodes, this->_start_node_indices[i], i, 0};
    PathRoutes start_path{{dummy_start_route}, i};
    PathUnit path_unit_start = {{this->_num_of_nodes, this->_start_node_indices[i]}, start_path};
    path_units.push_back(path_unit_start);

    Route dummy_end_route{this->_end_node_indices[i], this->_num_of_nodes, i, 0};
    PathRoutes end_path{{dummy_end_route}, i};
    PathUnit path_unit_end = {{this->_end_node_indices[i], this->_num_of_nodes}, end_path};
    path_units.push_back(path_unit_end);

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
    for (std::size_t i = 0; i < path_units.size(); ++i)
    {
      std::pair<std::size_t, std::size_t> pair = path_units.at(i).endpoints;
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
      PathRoutes middle_path{{e}, this->_num_of_vehicles};
      PathUnit path_unit_middle = {{e.prev, e.next}, middle_path};
      path_units.push_back(path_unit_middle);

      leave_node_quota[e.prev] -= 1;
      enter_node_quota[e.next] -= 1;

      this->_cost += e.cost;
    }
    // connect two path through edge e
    else if (prev_route_index != -1 && next_route_index != -1)
    {
      if (path_units[prev_route_index].path_route.vehicle < this->_num_of_vehicles &&
          path_units[next_route_index].path_route.vehicle < this->_num_of_vehicles)
      {
        if (path_units[prev_route_index].path_route.vehicle != path_units[next_route_index].path_route.vehicle)
        {
          continue;
        }
        else // path_units[prev_route_index].path_route.vehicle == path_units[next_route_index].path_route.vehicle
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

          PathRoutes finished_route = MergePathRoutes(path_units[prev_route_index].path_route,
                                                      path_units[next_route_index].path_route,
                                                      e,
                                                      path_units[prev_route_index].path_route.vehicle);

          complete_path_routes.push_back(finished_route);
          num_complete_path_routes += 1;

          path_units.erase(path_units.begin() + std::max(prev_route_index, next_route_index));
          path_units.erase(path_units.begin() + std::min(prev_route_index, next_route_index));
          leave_node_quota[e.prev] -= 1;
          enter_node_quota[e.next] -= 1;

          this->_cost += e.cost;

          if (num_complete_path_routes == this->_num_of_vehicles)
          {
            break;
          }
        }
      }
      else if (path_units[prev_route_index].path_route.vehicle == this->_num_of_vehicles &&
               path_units[next_route_index].path_route.vehicle == this->_num_of_vehicles)
      {
        // connect two paths that have not yet belonged to any vehicle

        PathRoutes connected_path = MergePathRoutes(path_units[prev_route_index].path_route,
                                                    path_units[next_route_index].path_route,
                                                    e,
                                                    this->_num_of_vehicles);

        PathUnit path_unit_middle = {{connected_path.routes.front().prev, connected_path.routes.back().next}, connected_path};
        path_units.push_back(path_unit_middle);

        path_units.erase(path_units.begin() + std::max(prev_route_index, next_route_index));
        path_units.erase(path_units.begin() + std::min(prev_route_index, next_route_index));
        leave_node_quota[e.prev] -= 1;
        enter_node_quota[e.next] -= 1;

        this->_cost += e.cost;
      }
      else if (path_units[next_route_index].path_route.vehicle == this->_num_of_vehicles)
      {
        // connect one path that has not yet belonged to any vehicle to a start path

        PathRoutes connected_path = MergePathRoutes(path_units[prev_route_index].path_route,
                                                    path_units[next_route_index].path_route,
                                                    e,
                                                    path_units[prev_route_index].path_route.vehicle);

        PathUnit path_unit_middle = {{connected_path.routes.front().prev, connected_path.routes.back().next}, connected_path};
        path_units.push_back(path_unit_middle);

        path_units.erase(path_units.begin() + std::max(prev_route_index, next_route_index));
        path_units.erase(path_units.begin() + std::min(prev_route_index, next_route_index));
        leave_node_quota[e.prev] -= 1;
        enter_node_quota[e.next] -= 1;

        this->_cost += e.cost;
      }
      else if (path_units[prev_route_index].path_route.vehicle == this->_num_of_vehicles)
      {
        // connect one path that has not yet belonged to any vehicle to an end path

        PathRoutes connected_path = MergePathRoutes(path_units[prev_route_index].path_route,
                                                    path_units[next_route_index].path_route,
                                                    e,
                                                    path_units[next_route_index].path_route.vehicle);

        PathUnit path_unit_middle = {{connected_path.routes.front().prev, connected_path.routes.back().next}, connected_path};
        path_units.push_back(path_unit_middle);

        path_units.erase(path_units.begin() + std::max(prev_route_index, next_route_index));
        path_units.erase(path_units.begin() + std::min(prev_route_index, next_route_index));
        leave_node_quota[e.prev] -= 1;
        enter_node_quota[e.next] -= 1;

        this->_cost += e.cost;
      }
    }
    // connect edge e to a start path
    else if (prev_route_index != -1)
    {
      path_units[prev_route_index].path_route.routes.push_back(e);
      path_units[prev_route_index].endpoints.second = e.next;

      leave_node_quota[e.prev] -= 1;
      enter_node_quota[e.next] -= 1;

      this->_cost += e.cost;
    }
    // connect edge e to an end path
    else if (next_route_index != -1)
    {
      path_units[next_route_index].path_route.routes.insert(path_units[next_route_index].path_route.routes.begin(), e);
      path_units[next_route_index].endpoints.first = e.prev;

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

void SimpleVRPSolver::InitialSolutionGlobalMaximumVarianceFirst()
{
  // Establish all edges and sort them
  std::vector<double> enter_square_sum(this->_num_of_nodes, 0);
  std::vector<double> leave_square_sum(this->_num_of_nodes, 0);
  std::vector<double> enter_sum(this->_num_of_nodes, 0);
  std::vector<double> leave_sum(this->_num_of_nodes, 0);

  struct VarianceRecord
  {
    std::size_t node_index;
    bool is_enter;
    double variance;
  };
  std::vector<VarianceRecord> enter_variance_list(this->_num_of_nodes, {0, true, 0});
  std::vector<VarianceRecord> leave_variance_list(this->_num_of_nodes, {0, false, 0});

  for (std::size_t i = 0; i < this->_num_of_nodes; ++i)
  {
    for (std::size_t j = 0; j < this->_num_of_nodes; ++j)
    {
      if (i != j)
      {
        Route temp_route{i, j, this->_num_of_vehicles, this->_cost_matrix[i][j]};
        double route_cost = this->_cost_matrix[i][j];

        enter_square_sum[j] += route_cost * route_cost;
        leave_square_sum[i] += route_cost * route_cost;
        enter_sum[j] += route_cost;
        leave_sum[i] += route_cost;
      }
    }
  }

  for (std::size_t i = 0; i < this->_num_of_nodes; ++i)
  {
    enter_variance_list[i].node_index = i;
    enter_variance_list[i].is_enter = true;
    enter_variance_list[i].variance = enter_square_sum[i] / (this->_num_of_nodes - 1) -
                                      (enter_sum[i] / (this->_num_of_nodes - 1)) * (enter_sum[i] / (this->_num_of_nodes - 1));

    leave_variance_list[i].node_index = i;
    leave_variance_list[i].is_enter = false;
    leave_variance_list[i].variance = leave_square_sum[i] / (this->_num_of_nodes - 1) -
                                      (leave_sum[i] / (this->_num_of_nodes - 1)) * (leave_sum[i] / (this->_num_of_nodes - 1));
  }

  // initialize m routes
  struct PathUnit
  {
    std::pair<std::size_t, std::size_t> endpoints;
    PathRoutes path_route;
  };
  std::vector<PathUnit> path_units;

  std::vector<std::size_t> enter_node_quota(this->_num_of_nodes, 0);
  std::vector<std::size_t> leave_node_quota(this->_num_of_nodes, 0);
  for (std::size_t i = 0; i < this->_num_of_vehicles; ++i)
  {
    Route dummy_start_route{this->_num_of_nodes, this->_start_node_indices[i], i, 0};
    PathRoutes start_path{{dummy_start_route}, i};
    PathUnit path_unit_start = {{this->_num_of_nodes, this->_start_node_indices[i]}, start_path};
    path_units.push_back(path_unit_start);

    Route dummy_end_route{this->_end_node_indices[i], this->_num_of_nodes, i, 0};
    PathRoutes end_path{{dummy_end_route}, i};
    PathUnit path_unit_end = {{this->_end_node_indices[i], this->_num_of_nodes}, end_path};
    path_units.push_back(path_unit_end);

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
  std::vector<std::vector<bool>> edge_selectable(
      this->_num_of_nodes, std::vector<bool>(this->_num_of_nodes, true));

  bool end_of_while = false;
  while (!end_of_while)
  {
    VarianceRecord max_variance_record = enter_variance_list.front();
    for (std::size_t i = 0; i < this->_num_of_nodes; ++i)
    {
      if (enter_node_quota[i] != 0 &&
          enter_variance_list[i].variance > max_variance_record.variance)
      {
        max_variance_record = enter_variance_list[i];
      }
      if (leave_node_quota[i] != 0 &&
          leave_variance_list[i].variance > max_variance_record.variance)
      {
        max_variance_record = leave_variance_list[i];
      }
    }

    int edge_from_index = -1;
    int edge_to_index = -1;
    double minimum_edge_cost = DBL_MAX;
    if (max_variance_record.is_enter)
    {
      for (std::size_t i = 0; i < this->_num_of_nodes; ++i)
      {
        if (leave_node_quota[i] == 0 ||
            i == max_variance_record.node_index ||
            enter_node_quota[max_variance_record.node_index] == 0 ||
            !edge_selectable[i][max_variance_record.node_index])
        {
          continue;
        }

        if (this->_cost_matrix[i][max_variance_record.node_index] < minimum_edge_cost)
        {
          minimum_edge_cost = this->_cost_matrix[i][max_variance_record.node_index];
          edge_from_index = i;
          edge_to_index = max_variance_record.node_index;
        }
      }
    }
    else
    {
      for (std::size_t i = 0; i < this->_num_of_nodes; ++i)
      {
        if (leave_node_quota[max_variance_record.node_index] == 0 ||
            max_variance_record.node_index == i ||
            enter_node_quota[i] == 0 ||
            !edge_selectable[max_variance_record.node_index][i])
        {
          continue;
        }

        if (this->_cost_matrix[max_variance_record.node_index][i] < minimum_edge_cost)
        {
          minimum_edge_cost = this->_cost_matrix[max_variance_record.node_index][i];
          edge_from_index = max_variance_record.node_index;
          edge_to_index = i;
        }
      }
    }

    bool occur_sub_cycle = false;
    int prev_route_index = -1;
    int next_route_index = -1;
    for (std::size_t i = 0; i < path_units.size(); ++i)
    {
      std::pair<std::size_t, std::size_t> pair = path_units.at(i).endpoints;
      if (edge_from_index == pair.second && edge_to_index == pair.first)
      {
        occur_sub_cycle = true;
        break;
      }

      if (edge_from_index == pair.second)
      {
        prev_route_index = i;
      }
      else if (edge_to_index == pair.first)
      {
        next_route_index = i;
      }
    }
    if (occur_sub_cycle)
    {
      edge_selectable[edge_from_index][edge_to_index] = false;
      continue;
    }

    Route e{edge_from_index, edge_to_index, this->_num_of_vehicles, minimum_edge_cost};
    // create a new path which is not connected to any other
    if (prev_route_index == -1 && next_route_index == -1)
    {
      PathRoutes middle_path{{e}, this->_num_of_vehicles};
      PathUnit path_unit_middle = {{e.prev, e.next}, middle_path};
      path_units.push_back(path_unit_middle);

      leave_node_quota[e.prev] -= 1;
      enter_node_quota[e.next] -= 1;

      this->_cost += e.cost;
    }
    // connect two path through edge e
    else if (prev_route_index != -1 && next_route_index != -1)
    {
      if (path_units[prev_route_index].path_route.vehicle < this->_num_of_vehicles &&
          path_units[next_route_index].path_route.vehicle < this->_num_of_vehicles)
      {
        if (path_units[prev_route_index].path_route.vehicle != path_units[next_route_index].path_route.vehicle)
        {
          edge_selectable[edge_from_index][edge_to_index] = false;
          continue;
        }
        else // path_units[prev_route_index].path_route.vehicle == path_units[next_route_index].path_route.vehicle
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
            edge_selectable[edge_from_index][edge_to_index] = false;
            continue;
          }

          PathRoutes finished_route = MergePathRoutes(path_units[prev_route_index].path_route,
                                                      path_units[next_route_index].path_route,
                                                      e,
                                                      path_units[prev_route_index].path_route.vehicle);

          complete_path_routes.push_back(finished_route);
          num_complete_path_routes += 1;

          path_units.erase(path_units.begin() + std::max(prev_route_index, next_route_index));
          path_units.erase(path_units.begin() + std::min(prev_route_index, next_route_index));
          leave_node_quota[e.prev] -= 1;
          enter_node_quota[e.next] -= 1;

          this->_cost += e.cost;

          if (num_complete_path_routes == this->_num_of_vehicles)
          {
            break;
          }
        }
      }
      else if (path_units[prev_route_index].path_route.vehicle == this->_num_of_vehicles &&
               path_units[next_route_index].path_route.vehicle == this->_num_of_vehicles)
      {
        // connect two paths that have not yet belonged to any vehicle

        PathRoutes connected_path = MergePathRoutes(path_units[prev_route_index].path_route,
                                                    path_units[next_route_index].path_route,
                                                    e,
                                                    this->_num_of_vehicles);

        PathUnit path_unit_middle = {{connected_path.routes.front().prev, connected_path.routes.back().next}, connected_path};
        path_units.push_back(path_unit_middle);

        path_units.erase(path_units.begin() + std::max(prev_route_index, next_route_index));
        path_units.erase(path_units.begin() + std::min(prev_route_index, next_route_index));
        leave_node_quota[e.prev] -= 1;
        enter_node_quota[e.next] -= 1;

        this->_cost += e.cost;
      }
      else if (path_units[next_route_index].path_route.vehicle == this->_num_of_vehicles)
      {
        // connect one path that has not yet belonged to any vehicle to a start path

        PathRoutes connected_path = MergePathRoutes(path_units[prev_route_index].path_route,
                                                    path_units[next_route_index].path_route,
                                                    e,
                                                    path_units[prev_route_index].path_route.vehicle);

        PathUnit path_unit_middle = {{connected_path.routes.front().prev, connected_path.routes.back().next}, connected_path};
        path_units.push_back(path_unit_middle);

        path_units.erase(path_units.begin() + std::max(prev_route_index, next_route_index));
        path_units.erase(path_units.begin() + std::min(prev_route_index, next_route_index));
        leave_node_quota[e.prev] -= 1;
        enter_node_quota[e.next] -= 1;

        this->_cost += e.cost;
      }
      else if (path_units[prev_route_index].path_route.vehicle == this->_num_of_vehicles)
      {
        // connect one path that has not yet belonged to any vehicle to an end path

        PathRoutes connected_path = MergePathRoutes(path_units[prev_route_index].path_route,
                                                    path_units[next_route_index].path_route,
                                                    e,
                                                    path_units[next_route_index].path_route.vehicle);

        PathUnit path_unit_middle = {{connected_path.routes.front().prev, connected_path.routes.back().next}, connected_path};
        path_units.push_back(path_unit_middle);

        path_units.erase(path_units.begin() + std::max(prev_route_index, next_route_index));
        path_units.erase(path_units.begin() + std::min(prev_route_index, next_route_index));
        leave_node_quota[e.prev] -= 1;
        enter_node_quota[e.next] -= 1;

        this->_cost += e.cost;
      }
    }
    // connect edge e to a start path
    else if (prev_route_index != -1)
    {
      path_units[prev_route_index].path_route.routes.push_back(e);
      path_units[prev_route_index].endpoints.second = e.next;

      leave_node_quota[e.prev] -= 1;
      enter_node_quota[e.next] -= 1;

      this->_cost += e.cost;
    }
    // connect edge e to an end path
    else if (next_route_index != -1)
    {
      path_units[next_route_index].path_route.routes.insert(path_units[next_route_index].path_route.routes.begin(), e);
      path_units[next_route_index].endpoints.first = e.prev;

      leave_node_quota[e.prev] -= 1;
      enter_node_quota[e.next] -= 1;

      this->_cost += e.cost;
    }

    for (std::size_t k = 0; k < this->_num_of_nodes; ++k)
    {
      if (e.prev != k)
      {
        double cost = this->_cost_matrix[e.prev][k];
        leave_sum[e.prev] -= cost;
        leave_square_sum[e.prev] -= cost * cost;
      }
      if (e.next != k)
      {
        double cost = this->_cost_matrix[k][e.next];
        enter_sum[e.next] -= cost;
        enter_square_sum[e.next] -= cost * cost;
      }
    }

    std::size_t denom = this->_num_of_nodes - 1;
    leave_variance_list[e.prev].variance =
        (leave_square_sum[e.prev] / denom) -
        std::pow(leave_sum[e.prev] / denom, 2);
    enter_variance_list[e.next].variance =
        (enter_square_sum[e.next] / denom) -
        std::pow(enter_sum[e.next] / denom, 2);

    if (leave_node_quota[e.prev] == 0)
    {
      leave_variance_list[e.prev].variance = -1.0;
    }
    if (enter_node_quota[e.next] == 0)
    {
      enter_variance_list[e.next].variance = -1.0;
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