package_add_test(graphs_gtest graphs_gtest.cpp
        ../../io/io.cpp
        ../../auxiliary/graphs.cpp
        ../../auxiliary/small_ccs.cpp
        )

package_add_test(math_gtest math_gtest.cpp
        )

package_add_test(biconnected_partition_gtest biconnected_partition_gtest.cpp
        ../../io/io.cpp
        ../../auxiliary/graphs.cpp
        ../../auxiliary/small_ccs.cpp
        ../../auxiliary/biconnected_partition.cpp
        )

package_add_test(mc_solution_gtest mc_solution_gtest.cpp
        ../../auxiliary/mc_solution.cpp
        )

package_add_test(sms_statistics_gtest statistics_gtest.cpp
        ../../io/io.cpp
        ../../auxiliary/graphs.cpp
        ../../auxiliary/small_ccs.cpp
        )

package_add_test(small_ccs_gtest small_ccs_gtest.cpp
        ../../io/io.cpp
        ../../auxiliary/graphs.cpp
        ../../auxiliary/small_ccs.cpp
        )

package_add_test(odd_closed_walk_gtest odd_closed_walk_gtest.cpp
        ../../auxiliary/odd_closed_walk.cpp
        )

package_add_test(ocw_separator_gtest ocw_separator_gtest.cpp
        )

package_add_test(two_separator_gtest two_separator_gtest.cpp
        ../../auxiliary/two_separator.cpp
        )

package_add_test(compact_graph_gtest compact_graph_gtest.cpp
        )

package_add_test(mst_heuristic_gtest mst_heuristic_gtest.cpp
        ../../auxiliary/mst_heuristic.cpp
        ../../auxiliary/mc_solution.cpp
        ../../auxiliary/odd_closed_walk.cpp
        )

package_add_test(kl_heuristic_gtest kl_heuristic_gtest.cpp
        ../../auxiliary/kl_heuristic.cpp
        ../../auxiliary/mc_solution.cpp
        ../../io/io.cpp
        ../../auxiliary/graphs.cpp
        ../../auxiliary/small_ccs.cpp
        )
