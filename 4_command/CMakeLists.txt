set(LIB_NAME command_lib)
set(TEST_NAME command_test)

add_library(${LIB_NAME} INTERFACE)

target_include_directories(${LIB_NAME}
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../2_game/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../3_exceptions/include
)

add_executable(${TEST_NAME} test/${TEST_NAME}.cpp)
target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${TEST_NAME} PRIVATE ${LIB_NAME} GTest::gtest_main)
target_compile_features(${TEST_NAME} PRIVATE cxx_std_20)

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
