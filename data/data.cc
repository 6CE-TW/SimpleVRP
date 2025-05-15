#include "data/data.h"

int GenerateRandomNumber(int min, int max)
{
  return rand() % (max - min + 1) + min;
}

void PrintNodeMap(std::vector<Cartesian> node_map, int iomanip_setting)
{
  if (iomanip_setting == 0)
  {
    for (const auto &n : node_map)
    {
      std::cout << "(" << n.x << ", " << n.y << ")\n";
    }
  }
  else
  {
    /* code */
  }
}

void PrintCostMatrix(std::vector<std::vector<double>> cost_matrix, int iomanip_setting)
{
  if (iomanip_setting == 0)
  {
    for (std::size_t i = 0; i < cost_matrix.size(); ++i)
    {
      for (std::size_t j = 0; j < cost_matrix.size(); ++j)
      {
        std::cout << cost_matrix[i][j] << " ";
      }
      std::cout << "\n";
    }
    std::cout << "\n";
  }
  else
  {
    for (std::size_t i = 0; i < cost_matrix.size(); ++i)
    {
      for (std::size_t j = 0; j < cost_matrix.size(); ++j)
      {
        std::cout << std::setw(iomanip_setting) << std::fixed << std::setprecision(2) << cost_matrix[i][j] << " ";
      }
      std::cout << "\n";
    }
    std::cout << "\n";
  }
}

std::vector<Cartesian> CreateNodeMap(int num_of_node, Cartesian sand_box)
{
  std::vector<Cartesian> node_map;
  for (size_t i = 0; i < num_of_node; i++)
  {
    int rand_x = GenerateRandomNumber(0, sand_box.x);
    int rand_y = GenerateRandomNumber(0, sand_box.y);
    Cartesian temp_node = {rand_x, rand_y};
    node_map.push_back(temp_node);
  }

  return node_map;
}

std::vector<std::vector<double>> CreateCostMatrix(int num_of_node, std::vector<Cartesian> node_map)
{
  std::vector<std::vector<double>> cost_matrix;

  for (std::size_t i = 0; i < num_of_node; ++i)
  {
    std::vector<double> cost_row;
    double n1_x = node_map.at(i).x;
    double n1_y = node_map.at(i).y;
    for (std::size_t j = 0; j < num_of_node; ++j)
    {
      if (i == j)
      {
        cost_row.push_back(0);
      }
      else
      {
        double n2_x = node_map.at(j).x;
        double n2_y = node_map.at(j).y;

        double distance = std::sqrt(std::pow((n1_x - n2_x), 2) + std::pow((n1_y - n2_y), 2));
        double biased_distance = distance * ((double)GenerateRandomNumber(800, 1200) / 1000);
        cost_row.push_back(biased_distance);
      }
    }
    cost_matrix.push_back(cost_row);
  }

  return cost_matrix;
}