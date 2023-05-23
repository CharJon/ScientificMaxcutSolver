
package_add_test(probdata_mc_rooted_gtest probdata_mc_rooted_gtest.cpp
        ../../io/io.cpp
        ../../auxiliary/graphs.cpp
        ../../auxiliary/small_ccs.cpp
        ../../auxiliary/mc_solution.cpp
        ../../auxiliary/scip.cpp
        ../../auxiliary/mst_heuristic.cpp
        ../../auxiliary/odd_closed_walk.cpp
        ../../branch/branchrule_degree.cpp
        ../../conshdlr/conshdlr_oscw.cpp
        ../../conshdlr/conshdlr_odd_subset_cycles.cpp
        ../../conshdlr/conshdlr_triangles.cpp
        ../../conshdlr/conshdlr_holes.cpp
        ../../eventhdlr/eventhdlr_history.cpp
        ../../eventhdlr/eventhdlr_root.cpp
        ../../pheur/pheur_mst.cpp
        ../../pheur/pheur_mqlib.cpp
        ../../probdata/mc_rooted.cpp
        )

package_add_test(probdata_mc_edges_only_gtest probdata_mc_edges_only_gtest.cpp
        ../../io/io.cpp
        ../../auxiliary/graphs.cpp
        ../../auxiliary/small_ccs.cpp
        ../../auxiliary/mc_solution.cpp
        ../../auxiliary/scip.cpp
        ../../auxiliary/mst_heuristic.cpp
        ../../auxiliary/odd_closed_walk.cpp
        ../../branch/branchrule_degree.cpp
        ../../conshdlr/conshdlr_oscw.cpp
        ../../conshdlr/conshdlr_odd_subset_cycles.cpp
        ../../conshdlr/conshdlr_triangles.cpp
        ../../conshdlr/conshdlr_holes.cpp
        ../../eventhdlr/eventhdlr_history.cpp
        ../../eventhdlr/eventhdlr_root.cpp
        ../../pheur/pheur_mst.cpp
        ../../pheur/pheur_mqlib.cpp
        ../../probdata/mc_edges_only.cpp
        )
