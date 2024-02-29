cmake_minimum_required(VERSION 3.2)

project(GameOfLife_MPI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(MPI REQUIRED)

add_executable(GameOfLife_MPI src/main.cpp src/game_of_life.cpp)