cmake_minimum_required(VERSION 2.8)
project(ics2015)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -MMD -Wall -Wno-tautological-constant-out-of-range-compare")



add_subdirectory(nemu)
