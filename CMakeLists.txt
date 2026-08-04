cmake_minimum_required(VERSION 3.24)

project(
    evoarch
    VERSION 0.1.0
    DESCRIPTION "System architecture simulator and optimization framework"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

option(BUILD_TESTING "Build unit tests" ON)
option(ENABLE_CLANG_TIDY "Run clang-tidy during compilation" OFF)

if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_PROGRAM clang-tidy)
    if(CLANG_TIDY_PROGRAM)
        set(CMAKE_CXX_CLANG_TIDY
            "${CLANG_TIDY_PROGRAM};-extra-arg=-Wno-unknown-warning-option")
    endif()
endif()

find_package(boost_graph CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_library(evoarch STATIC
    src/architecture/Architecture.cpp
    src/architecture/ArchitectureGraph.cpp
    src/architecture/ArchitectureLoader.cpp
    src/architecture/Component.cpp
    src/ga/Fitness.cpp
    src/ga/GeneticAlgorithm.cpp
    src/ga/Genome.cpp
    src/ga/Population.cpp
    src/services/ApiService.cpp
    src/services/LoadBalancer.cpp
    src/services/PostgresService.cpp
    src/services/RedisService.cpp
    src/simulation/Event.cpp
    src/simulation/Metrics.cpp
    src/simulation/Request.cpp
    src/simulation/RequestId.cpp
    src/simulation/Simulator.cpp
    src/simulation/Workflow.cpp
    src/simulation/Workload.cpp
)

add_library(evoarch::evoarch ALIAS evoarch)

target_include_directories(
    evoarch
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(
    evoarch
    PUBLIC
        Boost::graph
        nlohmann_json::nlohmann_json
)

target_compile_features(evoarch PUBLIC cxx_std_23)

add_executable(architecture_demo
    apps/architecture_demo/main.cpp
)

target_link_libraries(
    architecture_demo
    PRIVATE
        evoarch::evoarch
)

add_executable(architecture_loader_test
    apps/architecture_loader_test/main.cpp
)

target_link_libraries(
    architecture_loader_test
    PRIVATE
        evoarch::evoarch
)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

find_program(DOXYGEN_EXECUTABLE doxygen)
if(DOXYGEN_EXECUTABLE)
    add_custom_target(
        docs
        COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()
