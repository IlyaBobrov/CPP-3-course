cmake_minimum_required(VERSION 3.19)
project(projectConsist4)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lasan -fsanitize=address -g -O0")

add_executable(projectConsist4 main.cpp consist.hpp)