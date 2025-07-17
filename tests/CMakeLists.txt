# === tests/CMakeLists.txt ===

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(runTests ${TEST_SOURCES})

target_include_directories(runTests
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

target_link_libraries(runTests
    PRIVATE
    GTest::gtest_main
    GTest::gmock_main
    pokerlib
)

include(GoogleTest)
gtest_discover_tests(runTests)