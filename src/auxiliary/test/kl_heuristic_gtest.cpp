#include <gtest/gtest.h>

#include "sms/auxiliary/kl_heuristic.hpp"
#include "sms/io/io.hpp"
#include <filesystem>

TEST(KLHeuristic, ConstructorTest) {

    NetworKit::Graph g(4, true, false);

    g.addEdge(0, 1, 1);
    g.addEdge(1, 2, 1);
    g.addEdge(1, 3, 1);
    g.addEdge(2, 3, 1);

    KLHeuristic kl(&g);
}

TEST(KLHeuristic, SimpleTest) {

    NetworKit::Graph g(4, true, false);

    g.addEdge(0, 1, 1);
    g.addEdge(1, 2, 1);
    g.addEdge(1, 3, 1);
    g.addEdge(2, 3, 1);

    KLHeuristic kl(&g);

    kl.phase1Optimization();

    auto sol = kl.getPrimalSolution();

    ASSERT_EQ(sol.getCutValue(), 3);
    ASSERT_TRUE(sol.isValid());
}

TEST(KLHeuristic, SimpleWithInitSolTest) {

    NetworKit::Graph g(4, true, false);

    g.addEdge(0, 1, 1);
    g.addEdge(1, 2, 1);
    g.addEdge(1, 3, 1);
    g.addEdge(2, 3, 1);

    McSolution initSol(&g);
    initSol.toPartition0(0);
    initSol.toPartition1(1);
    initSol.toPartition1(2);
    initSol.toPartition1(3);

    ASSERT_EQ(initSol.getCutValue(), 1);

    KLHeuristic kl(&g);

    kl.phase1Optimization(initSol);

    auto sol = kl.getPrimalSolution();

    ASSERT_EQ(sol.getCutValue(), 3);
    ASSERT_TRUE(sol.isValid());
}

TEST(KLHeuristic, SimpleRepartitionTest) {

    NetworKit::Graph g(4, true, false);

    g.addEdge(0, 1, 1);
    g.addEdge(1, 2, 1);
    g.addEdge(1, 3, 1);
    g.addEdge(2, 3, 1);

    KLHeuristic kl(&g);

    kl.phase2Optimization();

    auto sol = kl.getPrimalSolution();

    ASSERT_EQ(sol.getCutValue(), 3);
    ASSERT_TRUE(sol.isValid());
}

TEST(KLHeuristic, ErrorCase)
{
    auto g = mcFileToGraph("test/data/w01_120.0.mc");
    KLHeuristic kl(&g);
    kl.phase2Optimization();
    auto sol = kl.getPrimalSolution();
    kl.printStats();
}