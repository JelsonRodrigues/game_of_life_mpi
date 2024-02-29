#pragma once

#include <cstdint>
#include <vector>
#include <tuple>
#include <cmath>

std::vector<std::vector<uint8_t>> create_grid(uint32_t rows, uint32_t cols);

std::tuple<std::vector<std::vector<uint8_t>>, size_t> update_rows(std::vector<std::vector<uint8_t>> grid, size_t row_begin, size_t number_of_rows);