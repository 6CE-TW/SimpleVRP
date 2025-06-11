#include <vector>
#include "solver/algorithm/initial_solution/initial_solution.h"

PathRoutes MergePathRoutes(
    const PathRoutes &first,
    const PathRoutes &second,
    const Route &connecting_edge,
    std::size_t assigned_vehicle)
{
  std::vector<Route> merged_routes;
  merged_routes.reserve(first.routes.size() + 1 + second.routes.size());

  merged_routes.insert(merged_routes.end(), first.routes.begin(), first.routes.end());
  merged_routes.push_back(connecting_edge);
  merged_routes.insert(merged_routes.end(), second.routes.begin(), second.routes.end());

  return PathRoutes{merged_routes, assigned_vehicle};
}
