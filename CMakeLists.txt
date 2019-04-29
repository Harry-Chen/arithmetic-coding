cmake_minimum_required(VERSION 3.10)
project(arithmetic_coding)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
find_package(GMP REQUIRED)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

add_executable(arithmetic_coding main.cpp)
target_link_libraries(arithmetic_coding gmp)