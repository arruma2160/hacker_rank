cmake_minimum_required(VERSION 3.18)

project(complex)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O0 -ggdb -Wall -Werror")

set(SOURCES tests_complex.cpp)
add_executable(run_tests ${SOURCES})
target_link_libraries(run_tests gtest pthread)
