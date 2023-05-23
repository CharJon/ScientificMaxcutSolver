#include <gtest/gtest.h>
#include <scip/scip.h>

#include "sms/io/io.hpp"
#include "sms/probdata/mc_rooted.hpp"
#include "sms/auxiliary/graphs.hpp"


TEST(ProbDataRootedMc, solveSquare) {
    NetworKit::Graph g = mcFileToGraph("test/data/square.mc");
    g.indexEdges();

    Scip *scip = nullptr;
    buildRootMcModel(&scip, &g, 0, false);
    ASSERT_NE(scip, nullptr);
    SCIPsolve(scip);
    SCIPfree(&scip);
}

TEST(ProbDataRootedMc, solvePm1s_50_1) {
    NetworKit::Graph g = mcFileToGraph("test/data/pm1s_50.1.mc");
    auto r = compactGraph(g);
    g = std::move(r.compact_graph);
    g.indexEdges();

    Scip *scip = nullptr;
    buildRootMcModel(&scip, &g, 1, false);
    ASSERT_NE(scip, nullptr);
    SCIPsolve(scip);
    SCIPfree(&scip);
}

TEST(ProbDataRootedMc, solvePm1s_50_1_plugins) {
    NetworKit::Graph g = mcFileToGraph("test/data/pm1s_50.1.mc");
    auto r = compactGraph(g);
    g = std::move(r.compact_graph);
    g.indexEdges();

    Scip *scip = nullptr;
    buildRootMcModel(&scip, &g, 1, false);
    ASSERT_NE(scip, nullptr);


    SCIPsolve(scip);


    SCIPfree(&scip);
}
