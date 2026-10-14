cmake_minimum_required(VERSION 3.31.6...4.4.0)

project(cmake_test VERSION 1.0 LANGUAGES CXX)

add_library(lib)

target_sources(
  lib
  PRIVATE
  src/lib.cpp
)

target_include_directories(
  lib
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)

add_subdirectory(test)
