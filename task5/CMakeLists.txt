cmake_minimum_required(VERSION 3.17)
project(task5)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER /opt/openmpi/bin/mpic++)

add_executable(task5 main.cpp)