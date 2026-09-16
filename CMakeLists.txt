cmake_minimum_required(VERSION 3.20.0)
project(lab2)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(loops loops.cpp)
add_executable(matrix_classic matrix_classic.cpp)
add_executable(matrix_block matrix_block.cpp)
