#include <gtest/gtest.h>

#include "sms/conshdlr/triangles.hpp"

TEST(TriangleSeparator, noTriangles) {
    NetworKit::Graph g(3, true);
    g.addEdge(0, 1, 0.4);
    g.addEdge(1, 2, 0.4);
    g.addEdge(2, 0, 0.4);

    auto ts = TriangleSeparator(&g);
    ts.updateWeights(0, 1, 0.3);
    ts.updateWeights(1, 2, 0.3);
    ts.updateWeights(2, 0, 0.3);

    auto res = ts.getViolatedTriangles();
    ASSERT_EQ(res.size(), 0);
}

TEST(TriangleSeparator, oneTriangle1) {
    NetworKit::Graph g(3, true);
    g.addEdge(0, 1, 0.4);
    g.addEdge(1, 2, 0.4);
    g.addEdge(2, 0, 0.4);
    auto ts = TriangleSeparator(&g);
    ts.addTriangle(0, 1, 2);
    ts.updateWeights(0, 1, 1);
    ts.updateWeights(1, 2, 1);
    ts.updateWeights(2, 0, 1);

    auto res = ts.getViolatedTriangles();
    ASSERT_EQ(res.size(), 1);
}

TEST(TriangleSeparator, oneTriangle2) {
    NetworKit::Graph g(3, true);
    g.addEdge(0, 1, 0.4);
    g.addEdge(1, 2, 0.4);
    g.addEdge(2, 0, 0.4);
    auto ts = TriangleSeparator(&g);
    ts.addTriangle(0, 1, 2);
    ts.updateWeights(0, 1, 0);
    ts.updateWeights(1, 2, 0);
    ts.updateWeights(2, 0, 1);

    auto res = ts.getViolatedTriangles();
    ASSERT_EQ(res.size(), 1);
}