cmake_minimum_required(VERSION 3.14)

set(TEST_NAME square_roots_test)
project(${TEST_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(googletest)

add_executable(${TEST_NAME} test/${TEST_NAME}.cpp)
target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${TEST_NAME} PRIVATE GTest::gtest_main)
target_compile_features(${TEST_NAME} PRIVATE cxx_std_20)

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
