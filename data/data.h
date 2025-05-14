#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <iomanip>
#include <vector>

#include <stdlib.h>
#include <ctime>

struct Cartesian
{
  double x;
  double y;
};

int GenerateRandomNumber(int min, int max);

void PrintNodeMap(std::vector<Cartesian> node_map, int iomanip_setting = 0);
void PrintCostMatrix(std::vector<std::vector<double>> cost_matrix, int iomanip_setting = 0);
std::vector<Cartesian> CreateNodeMap(int num_of_node, Cartesian sand_box);
std::vector<std::vector<double>> CreateCostMatrix(int num_of_node, std::vector<Cartesian> node_map);

#endif // DATA_H