cmake_minimum_required(VERSION 3.17)
project(task3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER /opt/openmpi/bin/mpic++)

add_executable(task3 main.cpp)