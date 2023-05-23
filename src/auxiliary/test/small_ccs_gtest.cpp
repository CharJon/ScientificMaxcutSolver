#include <gtest/gtest.h>

#include <vector>
#include "networkit/graph/Graph.hpp"
#include "sms/auxiliary/small_ccs.hpp"
#include "sms/io/io.hpp"
#include "sms/auxiliary/base.hpp"


TEST(smallCCs, sortFourCycles) {
    NetworKit::Graph g(5);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 0);

    g.addEdge(3, 4);

    auto r = sortFourCycle({0, 1, 2, 3}, g);
    fourcycle c = {1, 0, 3, 2};
    ASSERT_EQ(r, c);

    NetworKit::Graph g_2(4);
    g_2.addEdge(3, 1);
    g_2.addEdge(1, 0);
    g_2.addEdge(2, 0);
    g_2.addEdge(3, 2);

    r = sortFourCycle({0, 1, 2, 3}, g_2);
    c = {1, 0, 2, 3};
    ASSERT_EQ(r, c);

}

TEST(smallCCs, sortTriangles) {
    auto r = sortTriangle({1, 2, 3});
    triangle c = {2, 1, 3};
    ASSERT_EQ(r, c);

    r = sortTriangle({8, 9, 3});
    c = {8, 3, 9};
    ASSERT_EQ(r, c);
}

TEST(smallCCs, trippleSquareGraph) {
    NetworKit::Graph g = mcFileToGraph("test/data/tripleSquareTriangle.mc");
    g.indexEdges();

    SmallChordlessCycles ccs(g);

    ccs.run();

    ASSERT_TRUE(ccs.hasRun());
    ASSERT_EQ(ccs.triangles.size(), 2);
    ASSERT_EQ(ccs.fourHoles.size(), 2);

    ASSERT_TRUE(contains({6, 5, 7}, ccs.triangles));
    ASSERT_TRUE(contains({7, 5, 8}, ccs.triangles));

    ASSERT_TRUE(contains({2, 1, 4, 3}, ccs.fourHoles));
    ASSERT_TRUE(contains({4, 3, 6, 5}, ccs.fourHoles));
}


TEST(smallCCs, noTriangle) {
    NetworKit::Graph g = mcFileToGraph("test/data/square.mc");
    g.indexEdges();

    SmallChordlessCycles ccs(g);

    ccs.run();

    ASSERT_TRUE(ccs.hasRun());
    ASSERT_EQ(ccs.triangles.size(), 0);
    ASSERT_EQ(ccs.fourHoles.size(), 1);
}


TEST(smallCCs, noFourHoles) {
    NetworKit::Graph g = mcFileToGraph("test/data/two_triangles.mc");
    g.indexEdges();

    SmallChordlessCycles ccs(g);

    ccs.run();

    ASSERT_TRUE(ccs.hasRun());
    ASSERT_EQ(ccs.triangles.size(), 2);
    ASSERT_EQ(ccs.fourHoles.size(), 0);
}

TEST(smallCCs, nothing) {
    NetworKit::Graph g = mcFileToGraph("test/data/double_star.mc");
    g.indexEdges();

    SmallChordlessCycles ccs(g);

    ccs.run();

    ASSERT_TRUE(ccs.hasRun());
    ASSERT_EQ(ccs.triangles.size(), 0);
    ASSERT_EQ(ccs.fourHoles.size(), 0);
}

TEST(smallCCs, caNetsceince) {
    NetworKit::Graph g = mcFileToGraph("test/data/ca-netscience.mc");
    g.indexEdges();

    SmallChordlessCycles ccs(g);

    ccs.run();

    ASSERT_TRUE(ccs.hasRun());

    ASSERT_EQ(ccs.triangles.size(), 921);
}


TEST(smallCCs, egoFacebook) {
    NetworKit::Graph g = mcFileToGraph("test/data/ego-facebook.mc");
    g.indexEdges();

    SmallChordlessCycles ccs(g);

    ccs.run();

    ASSERT_TRUE(ccs.hasRun());
    ASSERT_EQ(ccs.triangles.size(), 273 / 3);
}

TEST(smallCCs, complete) {
    int c = 100;
    NetworKit::Graph g(c);

    for (int i = 0; i < c; i++) {
        for (int j = i + 1; j < c; j++) {
            g.addEdge(i, j);
        }
    }

    g.indexEdges();

    SmallChordlessCycles ccs(g);

    ccs.run();

    ASSERT_TRUE(ccs.hasRun());
    ASSERT_EQ(ccs.triangles.size(), c * (c - 1) * (c - 2) / 6);
}