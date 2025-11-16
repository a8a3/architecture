set(LIB_NAME exceptions_lib)
set(TEST_NAME exceptions_test)

add_library(${LIB_NAME}
    src/exceptions_impl.cpp
    src/command_impl.cpp
)

target_include_directories(exceptions_lib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(${TEST_NAME} test/${TEST_NAME}.cpp)
target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${TEST_NAME} PRIVATE ${LIB_NAME} GTest::gtest_main)
target_compile_features(${TEST_NAME} PRIVATE cxx_std_20)

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
