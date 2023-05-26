cmake_minimum_required(VERSION 3.3)
set(CMAKE_CXX_STANDARD 20)

project(sms)

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif ()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fopenmp -llapack")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fopenmp -DNDEBUG -llapack")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "") # Enable/Disable output of compile commands during generation


### This Project #####
add_subdirectory(src)

# NO hpp files needed here!
# Sort alphabetical (leave main at position 0)
set(SOURCES
        src/mains/main.cpp
        src/auxiliary/cl_parser.cpp
        src/auxiliary/ocw_seperator.cpp
        src/auxiliary/mc_solution.cpp
        src/auxiliary/scip.cpp
        src/auxiliary/small_ccs.cpp
        src/auxiliary/two_separator.cpp
        src/auxiliary/graphs.cpp
        src/auxiliary/biconnected_partition.cpp
        src/auxiliary/mst_heuristic.cpp
        src/auxiliary/odd_closed_walk.cpp
        src/branch/branchrule_degree.cpp
        src/branch/branchrule_pscost.cpp
        src/branch/branchrule_nm.cpp
        src/conshdlr/conshdlr_triangles.cpp
        src/conshdlr/conshdlr_holes.cpp
        #src/conshdlr/conshdlr_odd_subset_cycles.cpp
        src/conshdlr/conshdlr_oscw.cpp
        src/eventhdlr/eventhdlr_history.cpp
        src/eventhdlr/eventhdlr_root.cpp
        src/instance/maxcut.cpp
        src/instance/ising.cpp
        src/io/io.cpp
        src/io/dimacs_reader.cpp
        src/io/ising_reader.cpp
        src/pheur/pheur_mst.cpp
        src/pheur/pheur_mqlib.cpp
        src/probdata/mc_rooted.cpp
        src/probdata/mc_edges_only.cpp
        src/solver/basic_ocw.cpp
        src/auxiliary/kl_heuristic.cpp
        )

add_executable(sms ${SOURCES})

target_include_directories(sms
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        )

### Checks exe ####

set(SOURCES_CHECKS
        src/mains/checks.cpp
        src/io/io.cpp)

add_executable(checks ${SOURCES_CHECKS})

target_include_directories(checks
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        )

### MQLib exe ####

set(SOURCES_PHEURISTIC
        src/mains/mqlib.cpp
        src/io/io.cpp
        )

add_executable(pheuristic ${SOURCES_PHEURISTIC})

target_include_directories(pheuristic
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        )

### Extern Projects #####

# NetworKIT
if (NOT NETWORKIT_DIR)
    add_subdirectory(extern/networkit)
else ()
    add_library(networkit SHARED IMPORTED)
    set_target_properties(networkit PROPERTIES
            IMPORTED_LOCATION ${NETWORKIT_DIR}/lib/libnetworkit.so
            INTERFACE_INCLUDE_DIRECTORIES ${NETWORKIT_DIR}/include
            )
endif ()

# SCIP
# Note: SCIP_DIR as a path is only a recommendation for cmake
set(SCIP_DIR "$ENV{SCIP_DIR}" CACHE PATH "Path to SCIP directory")
message(STATUS "Will use SCIP_DIR ${SCIP_DIR}.")
find_package(SCIP PATHS ${SCIP_DIR} NO_DEFAULT_PATH REQUIRED)
message(STATUS "Using SCIP library from here ${SCIP_DIR}.")

# json
add_subdirectory(extern/json)

#cxxopts
add_subdirectory(extern/cxxopts)

# mqlib
add_subdirectory(extern/MQLib)

# BLAS
find_package(BLAS REQUIRED)


# gtest
option(TESTS "Build the tests" ON)
if (TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory("${PROJECT_SOURCE_DIR}/extern/gtest" "extern/googletest")

    macro(package_add_test TESTNAME)
        # create an executable in which the tests will be stored
        add_executable(${TESTNAME} ${ARGN})
        # link the Google test infrastructure, mocking library, and a default main function to
        # the test executable.  Remove g_test_main if writing your own main function.
        target_include_directories(${TESTNAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
        target_link_libraries(${TESTNAME} gtest gmock gtest_main ${SCIP_LIBRARIES} ${BLAS_LIBRARIES} networkit mqlib nlohmann_json::nlohmann_json cxxopts)
        # gtest_discover_tests replaces gtest_add_tests,
        # see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it
        gtest_discover_tests(${TESTNAME}
                # set a working directory so your project root so that you can find test data via paths relative to the project root
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
                )
        set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
    endmacro()

    # ADD TEST DIRECTORIES HERE
    add_subdirectory(${PROJECT_SOURCE_DIR}/src/auxiliary/test)
    add_subdirectory(${PROJECT_SOURCE_DIR}/src/branch/test)
    add_subdirectory(${PROJECT_SOURCE_DIR}/src/conshdlr/test)
    add_subdirectory(${PROJECT_SOURCE_DIR}/src/pheur/test)
    add_subdirectory(${PROJECT_SOURCE_DIR}/src/probdata/test)
    add_subdirectory(${PROJECT_SOURCE_DIR}/src/instance/test)
    add_subdirectory(${PROJECT_SOURCE_DIR}/src/solver/test)
endif ()

# gbenchmmark
option(MICROBENCH "Build microbenchmarks" OFF)
if (MICROBENCH)
    set(BENCHMARK_ENABLE_TESTING OFF)
    add_subdirectory("${PROJECT_SOURCE_DIR}/extern/gbenchmark" extern/gbenchmark)
    add_subdirectory(micro_benchmark)
endif ()


## This project links

target_link_libraries(sms
        ${SCIP_LIBRARIES}
        ${BLAS_LIBRARIES}
        cxxopts
        networkit
        nlohmann_json::nlohmann_json
        mqlib
        )

target_link_libraries(checks
        networkit
        )

target_link_libraries(pheuristic
        ${BLAS_LIBRARIES}
        networkit
        mqlib
        )

## install
message(STATUS ${CMAKE_INSTALL_PREFIX})
install(TARGETS sms RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})


# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()



