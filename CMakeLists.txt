cmake_minimum_required(VERSION 3.20)
project(timestorm VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(TIMESTORM_BUILD_TESTS "Build the test suite" OFF)
option(TIMESTORM_ENABLE_COVERAGE "Enable code coverage instrumentation (requires GCC/Clang)" OFF)

add_library(timestorm STATIC
  timestorm/timer.cpp
)
target_include_directories(timestorm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(TIMESTORM_ENABLE_COVERAGE)
  target_compile_options(timestorm PRIVATE --coverage -O0 -g)
  target_link_options(timestorm PRIVATE --coverage)
endif()

if(TIMESTORM_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
