cmake_minimum_required(VERSION 3.10)

project(hrml VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O0 -ggdb -Wall -Werror")

set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)


include_directories(.)

set(SOURCES node.cpp hrml.cpp)
add_library(hrml STATIC ${SOURCES})

set(TEST "tests/tests.cpp")
add_executable(run_tests ${TEST})
target_link_libraries(run_tests gtest hrml pthread)

add_executable(hacker_rank ./hacker_rank.cpp)
