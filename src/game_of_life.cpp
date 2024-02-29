#include "./include/game_of_life.h"

std::vector<std::vector<uint8_t>> create_grid(uint32_t rows, uint32_t cols) {
  std::vector<std::vector<uint8_t>> grid(rows, std::vector<uint8_t>(cols, 0));
  return grid;
}

std::tuple<std::vector<std::vector<uint8_t>>, size_t> update_rows(
  std::vector<std::vector<uint8_t>> grid, 
  size_t row_begin, 
  size_t number_of_rows
  )
{
  std::vector<std::vector<uint8_t>> new_grid = create_grid(number_of_rows, grid[0].size());

  for (size_t i = row_begin; i < row_begin + number_of_rows; ++i) {
    for (size_t j = 0; j < grid[i].size(); ++j) {
      uint8_t sum = 0;

      for (size_t k = std::max((int64_t)0, (int64_t)i - (int64_t)1); k < std::min(grid.size(), i + (size_t)2); ++k) {
        for (size_t l = std::max((int64_t)0, (int64_t)j - (int64_t)1); l < std::min(grid[i].size(), j + (size_t)2); ++l) {
          sum += grid[k][l];
        }
      }

      new_grid[i - row_begin][j] = sum == 2 || 
                       sum == 3 ||
                       sum == 4;
    }
  }

  return std::make_tuple(new_grid, row_begin);
}

