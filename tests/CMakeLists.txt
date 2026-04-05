include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.14.0
)
FetchContent_MakeAvailable(Catch2)

add_executable(timestorm_tests
  test_timer.cpp
)

target_link_libraries(timestorm_tests PRIVATE
  timestorm
  Catch2::Catch2WithMain
)

if(TIMESTORM_ENABLE_COVERAGE)
  target_compile_options(timestorm_tests PRIVATE --coverage -O0 -g)
  target_link_options(timestorm_tests PRIVATE --coverage)
endif()

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(timestorm_tests)
