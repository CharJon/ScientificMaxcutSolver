#include <gtest/gtest.h>
#include <scip/scip.h>

#include "sms/io/io.hpp"
#include "sms/probdata/mc_edges_only.hpp"
#include "sms/auxiliary/graphs.hpp"
#include "sms/conshdlr/conshdlr_oscw.hpp"
#include "sms/conshdlr/conshdlr_triangles.hpp"
#include "sms/conshdlr/conshdlr_holes.hpp"


TEST(ProbDataEdgesOnlyMc, solveSquare) {
    NetworKit::Graph g = mcFileToGraph("test/data/square.mc");
    g.indexEdges();

    Scip *scip = nullptr;
    buildEdgesOnlyMcModel(&scip, &g);
    ASSERT_NE(scip, nullptr);
    SCIPsolve(scip);
    SCIPfree(&scip);
}

TEST(ProbDataEdgesOnlyMc, solvePm1s_50_1) {
    NetworKit::Graph g = mcFileToGraph("test/data/pm1s_50.1.mc");
    auto r = compactGraph(g);
    g = std::move(r.compact_graph);
    g.indexEdges();

    Scip *scip = nullptr;
    buildEdgesOnlyMcModel(&scip, &g);
    ASSERT_NE(scip, nullptr);
    SCIPsolve(scip);
    SCIPfree(&scip);
}

TEST(ProbDataEdgesOnlyMc, solvePm1s_50_1_plugins) {
    NetworKit::Graph g = mcFileToGraph("test/data/pm1s_50.1.mc");
    g.removeNode(0);
    {
        auto r = compactGraph(g);
        g = std::move(r.compact_graph);
    }
    g.indexEdges();
    printGraphInformation(g);


    Scip *scip = nullptr;
    buildEdgesOnlyMcModel(&scip, &g);
    ASSERT_NE(scip, nullptr);

    SCIPsetIntParam(scip, "presolving/maxrounds", 0);
    SCIPsetIntParam(scip, "presolving/maxrestarts", 0);

    auto *ocwFastHandler = new ConshdlrOscw(scip, &g);
    SCIP_CALL_EXC(SCIPincludeObjConshdlr(scip, ocwFastHandler, TRUE));
    // Add a constraint for the odd cycle ocwHandler, to trigger callbacks to it
    SCIP_CONS *cons;
    SCIP_CALL_EXC(SCIPcreateConsBasicOddCyclesFast(scip, &cons, "Odd Cycles", &g))
    SCIP_CALL_EXC(SCIPaddCons(scip, cons))
    SCIP_CALL_EXC(SCIPreleaseCons(scip, &cons))

    SCIPsolve(scip);
    SCIPfree(&scip);
}

TEST(DISABLED_ProbDataEdgesOnlyMc, solvePm1s_100_3_plugins) {
    NetworKit::Graph g = mcFileToGraph("test/data/pm1s_100.3.mc");
    g.removeNode(0);
    {
        auto r = compactGraph(g);
        g = std::move(r.compact_graph);
    }
    g.indexEdges();
    printGraphInformation(g);

    Scip *scip = nullptr;
    buildEdgesOnlyMcModel(&scip, &g);
    ASSERT_NE(scip, nullptr);

    SCIPsetIntParam(scip, "presolving/maxrounds", 0);
    SCIPsetIntParam(scip, "presolving/maxrestarts", 0);
    turnOffAllCuts(scip);
    turnOffAllPrimalHeuristics(scip);
    turnOffConflictAnalysis(scip);

    auto *ocwFastHandler = new ConshdlrOscw(scip, &g);
    SCIP_CALL_EXC(SCIPincludeObjConshdlr(scip, ocwFastHandler, TRUE));
    // Add a constraint for the odd cycle ocwHandler, to trigger callbacks to it
    SCIP_CONS *cons;
    SCIP_CALL_EXC(SCIPcreateConsBasicOddCyclesFast(scip, &cons, "Odd Cycles", &g))
    SCIP_CALL_EXC(SCIPaddCons(scip, cons))
    SCIP_CALL_EXC(SCIPreleaseCons(scip, &cons))

    // Add triangle handler
    auto *triangleHandler = new ConshdlrTriangles(scip, &g);
    SCIP_CALL_EXC(SCIPincludeObjConshdlr(scip, triangleHandler, TRUE))

    // Add hole handler
    auto *holeHandler = new ConshdlrHoles(scip, &g);
    SCIP_CALL_EXC(SCIPincludeObjConshdlr(scip, holeHandler, TRUE))

    SCIPsolve(scip);
    SCIPprintStatistics(scip, nullptr);

    SCIPfree(&scip);
}
