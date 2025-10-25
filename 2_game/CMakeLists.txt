set(TEST_NAME game_test)

add_executable(${TEST_NAME} test/${TEST_NAME}.cpp)
target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${TEST_NAME} PRIVATE GTest::gtest_main)
target_compile_features(${TEST_NAME} PRIVATE cxx_std_20)

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
