find_package(GTest CONFIG REQUIRED)

add_executable(evoarch_tests
    smoke_test.cpp
)

target_link_libraries(
    evoarch_tests
    PRIVATE
        evoarch::evoarch
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(evoarch_tests)
