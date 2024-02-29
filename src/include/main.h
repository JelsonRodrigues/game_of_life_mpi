#pragma once

#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include <chrono>
#include <thread>
#include <vector>

void distributeWork(const std::vector<std::vector<uint8_t>> &matrix, int32_t worldSize);
void gatherResults(std::vector<std::vector<uint8_t>> &matrix, int32_t worldSize);
void print_matrix(const std::vector<std::vector<uint8_t>> &matrix);

void sendMatrixMPI(const std::vector<std::vector<uint8_t>> &matrix, int32_t destiny);
std::vector<std::vector<uint8_t>> receiveMatrixMPI(int32_t source);
