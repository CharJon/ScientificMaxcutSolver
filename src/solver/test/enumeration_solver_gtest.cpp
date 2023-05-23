#include <gtest/gtest.h>

#include <vector>
#include "networkit/graph/Graph.hpp"
#include "networkit/io/EdgeListReader.hpp"
#include "sms/solver/enumeration_solver.hpp"
#include "sms/io/io.hpp"

NetworKit::Graph squareGraph() {
    auto er = NetworKit::EdgeListReader();
    auto g = er.read("test/data/square.wel");
    assert(g.isWeighted());
    return g;
}

NetworKit::Graph cliqueGraph(int n) {
    assert(n > 0);
    auto g = NetworKit::Graph(n);

    for (auto u: g.nodeRange()) {
        for (auto v: g.nodeRange()) {
            if (u < v) g.addEdge(u, v);
        }
    }
    
    return g;
}

TEST(Enumeration, CutValue) {
    NetworKit::Graph g = squareGraph();

    EnumerationSolver solver(&g);

    uint cut = 0;
    ASSERT_EQ(solver.getCutValue(cut), 0);

    cut = 15;
    ASSERT_EQ(solver.getCutValue(cut), 0);

    cut = 1;
    ASSERT_EQ(solver.getCutValue(cut), -0.7);

    cut = 14;
    ASSERT_EQ(solver.getCutValue(cut), -0.7);

    cut = 9;
    ASSERT_EQ(solver.getCutValue(cut), 1.1);
}

TEST(Enumeration, CutValueParallel) {
    NetworKit::Graph g = mcFileToGraph("test/data/square.mc");

    EnumerationSolver solver(&g);

    uint cut = 0;
    ASSERT_EQ(solver.parallelGetCutValue(cut), 0);

    cut = 30;
    ASSERT_EQ(solver.parallelGetCutValue(cut), 0);

    cut = 2;
    ASSERT_EQ(solver.parallelGetCutValue(cut), -0.7);

    cut = 28;
    ASSERT_EQ(solver.parallelGetCutValue(cut), -0.7);

    cut = 18;
    ASSERT_EQ(solver.parallelGetCutValue(cut), 1.1);
}

TEST(Enumeration, SolvingSimple) {
    NetworKit::Graph g = mcFileToGraph("test/data/square.mc");

    EnumerationSolver solver(&g);

    solver.solve();

    ASSERT_EQ(solver.getCutValue(solver.bestPartition), solver.bestValue);
    ASSERT_EQ(solver.bestValue, 1.1);

}

TEST(Enumeration, SolvingComplex) {
    NetworKit::Graph g = mcFileToGraph("test/data/easy_20_vertices.mc");

    EnumerationSolver solver(&g);

    solver.solve();

    ASSERT_EQ(solver.getCutValue(solver.bestPartition), solver.bestValue);
    ASSERT_EQ(solver.bestValue, 59);
}

TEST(Enumeration, SolvingClique10) {
    auto g = cliqueGraph(10);
    EnumerationSolver solver(&g);
    solver.solve();

    ASSERT_EQ(solver.getCutValue(solver.bestPartition), solver.bestValue);
    ASSERT_EQ(solver.bestValue, 25);
}

TEST(Enumeration, SolvingClique19) {
    auto g = cliqueGraph(19);
    EnumerationSolver solver(&g);
    solver.solve();

    ASSERT_EQ(solver.getCutValue(solver.bestPartition), solver.bestValue);
    ASSERT_EQ(solver.bestValue, 90);
}

TEST(Enumeration, SolvingClique20) {
    auto g = cliqueGraph(20);
    EnumerationSolver solver(&g);
    solver.solve();

    ASSERT_EQ(solver.getCutValue(solver.bestPartition), solver.bestValue);
    ASSERT_EQ(solver.bestValue, 100);
}

TEST(Enumeration, SolvingComplexParallel) {
    NetworKit::Graph g = mcFileToGraph("test/data/easy_20_vertices.mc");

    EnumerationSolver solver(&g);

    solver.solve();

    ASSERT_EQ(solver.parallelGetCutValue(solver.bestPartition), solver.bestValue);
    ASSERT_EQ(solver.bestValue, 59);

}


TEST(Enumeration, SolvingSimpleFixedLast1) {
    NetworKit::Graph g = squareGraph();

    EnumerationSolver solver(&g, {3}, {true});

    solver.solve();

    ASSERT_EQ(solver.bestValue, 1.1);

}

TEST(Enumeration, SolvingSimpleFixedLast2) {
    NetworKit::Graph g = squareGraph();

    EnumerationSolver solver(&g, {3}, {false});

    solver.solve();

    ASSERT_EQ(solver.bestValue, 1.1);
}

TEST(Enumeration, SolvingSimpleFixedFirst1) {
    NetworKit::Graph g = squareGraph();

    EnumerationSolver solver(&g, {0}, {true});

    solver.solve();

    ASSERT_EQ(solver.getCutValue(solver.bestPartition), solver.bestValue);
    ASSERT_EQ(solver.bestValue, 1.1);
}

TEST(Enumeration, SolvingSimpleFixedFirst2) {
    NetworKit::Graph g = squareGraph();
    EnumerationSolver solver(&g, {0}, {false});

    solver.solve();

    ASSERT_EQ(solver.getCutValue(solver.bestPartition), solver.bestValue);
    ASSERT_EQ(solver.bestValue, 1.1);

}


TEST(Enumeration, SolvingSimpleFixedMulti1) {
    NetworKit::Graph g = squareGraph();
    EnumerationSolver solver(&g, {1, 2}, {false, true});

    solver.solve();

    ASSERT_EQ(solver.getCutValue(solver.bestPartition), solver.bestValue);
    ASSERT_EQ(solver.bestValue, -1.4);
}

TEST(Enumeration, SolvingSimpleFixedMulti2) {
    NetworKit::Graph g = squareGraph();
    EnumerationSolver solver(&g, {1, 2}, {true, true});

    solver.solve();

    ASSERT_EQ(solver.getCutValue(solver.bestPartition), solver.bestValue);
    ASSERT_EQ(solver.bestValue, 1.1);
}

TEST(Enumeration, SolvingSimpleFixedMulti3) {
    NetworKit::Graph g = squareGraph();
    EnumerationSolver solver(&g, {1, 2}, {false, false});

    solver.solve();

    ASSERT_EQ(solver.getCutValue(solver.bestPartition), solver.bestValue);
    ASSERT_EQ(solver.bestValue, 1.1);

}


TEST(Enumeration, SolvingSimpleFixedAlmostAll1) {
    NetworKit::Graph g = squareGraph();
    EnumerationSolver solver(&g, {0, 2, 3}, {true, false, true});

    solver.solve();

    ASSERT_EQ(solver.getCutValue(solver.bestPartition), solver.bestValue);
    ASSERT_EQ(solver.bestValue, 1.1);
}

TEST(Enumeration, SolvingSimpleFixedAlmostAll2) {
    NetworKit::Graph g = squareGraph();
    EnumerationSolver solver(&g, {0, 1, 2}, {true, true, true});

    solver.solve();

    ASSERT_EQ(solver.getCutValue(solver.bestPartition), solver.bestValue);
    ASSERT_EQ(solver.bestValue, 0);

}

TEST(Enumeration, SolvingComplexFixed1) {
    NetworKit::Graph g = mcFileToGraph("test/data/easy_20_vertices.mc");

    EnumerationSolver solver(&g, {1}, {true});

    solver.solve();

    ASSERT_EQ(solver.getCutValue(solver.bestPartition), solver.bestValue);
    ASSERT_EQ(solver.bestValue, 59);
}

TEST(Enumeration, SolvingComplexFixed2) {
    NetworKit::Graph g = mcFileToGraph("test/data/easy_20_vertices.mc");

    EnumerationSolver solver(&g, {0, 2}, {true, false});

    solver.solve();

    ASSERT_EQ(solver.getCutValue(solver.bestPartition), solver.bestValue);
    ASSERT_EQ(solver.bestValue, 59);
}

TEST(Enumeration, SolvingComplexFixed3) {
    NetworKit::Graph g = mcFileToGraph("test/data/easy_20_vertices.mc");

    EnumerationSolver solver(&g, {0, 2, 3}, {true, false, true});

    solver.solve();

    ASSERT_EQ(solver.getCutValue(solver.bestPartition), solver.bestValue);
    ASSERT_EQ(solver.bestValue, 59);
}
