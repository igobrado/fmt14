cmake_minimum_required(VERSION 3.28.3 FATAL_ERROR)

project(
  fmt
  VERSION 0.1
  LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PROJECT_DESCRIPTION "CTC formatting library")

option(ENABLE_EXAMPLE_BUILD "Builds the example project" ON)
option(ENABLE_TESTING "Builds unit tests" ON)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/tests)

