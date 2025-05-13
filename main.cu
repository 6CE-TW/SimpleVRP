#include <iostream>
#include <vector>

#include "data.h"

int main()
{
  int n = 10;
  Cartesian sand_box = {100, 100};
  std::vector<Cartesian> node_map = CreateNodeMap(n, sand_box);
  PrintNodeMap(node_map);

  std::vector<std::vector<double>> cost_matrix = CreateCostMatrix(n, node_map);
  PrintCostMatrix(cost_matrix, 5);

  return 0;
}