set(LIB_NAME ioc_lib)
add_library(${LIB_NAME} INTERFACE)

target_include_directories(${LIB_NAME}
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../2_game/include
)

set(TEST_NAME ioc_test)
add_executable(${TEST_NAME} test/${TEST_NAME}.cpp)
target_link_libraries(${TEST_NAME} PRIVATE ${LIB_NAME} GTest::gtest_main)
target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(${TEST_NAME} PRIVATE cxx_std_20)

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
