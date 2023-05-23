add_subdirectory("${PROJECT_SOURCE_DIR}/extern/gbenchmark" extern/gbenchmark)

add_executable(bench_cache cache.cpp)
target_include_directories(bench_cache
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        )
target_link_libraries(bench_cache
        benchmark::benchmark)


add_executable(bench_cg compact_graph.cpp)
target_include_directories(bench_cg
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/extern/networkit/include
        )
target_link_libraries(bench_cg
        benchmark::benchmark
        networkit
        )