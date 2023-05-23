package_add_test(basic_ocw_gtest basic_ocw_gtest.cpp
        ../../io/io.cpp
        ../../auxiliary/scip.cpp
        ../../auxiliary/graphs.cpp
        ../../auxiliary/small_ccs.cpp
        ../../auxiliary/mc_solution.cpp
        ../../auxiliary/mst_heuristic.cpp
        ../../auxiliary/odd_closed_walk.cpp
        ../../branch/branchrule_degree.cpp
        ../../branch/branchrule_nm.cpp
        ../../conshdlr/conshdlr_oscw.cpp
        ../../conshdlr/conshdlr_triangles.cpp
        ../../conshdlr/conshdlr_holes.cpp
        ../../conshdlr/conshdlr_odd_subset_cycles.cpp
        ../../eventhdlr/eventhdlr_history.cpp
        ../../eventhdlr/eventhdlr_root.cpp
        ../../pheur/pheur_mst.cpp
        ../../pheur/pheur_mqlib.cpp
        ../../probdata/mc_rooted.cpp
        ../../probdata/mc_edges_only.cpp
        ../../solver/basic_ocw.cpp
        )

package_add_test(enumeration_solver_gtest enumeration_solver_gtest.cpp
        ../../io/io.cpp
        ../../auxiliary/graphs.cpp
        ../../auxiliary/small_ccs.cpp
        ../../auxiliary/mc_solution.cpp
        ../../solver/enumeration_solver.cpp
        )

package_add_test(basic_qubo_enumeration_solver_gtest basic_qubo_enumeration_solver_gtest.cpp
        ../../instance/qubo.cpp
        ../../solver/basic_qubo_enumeration_solver.cpp
        )