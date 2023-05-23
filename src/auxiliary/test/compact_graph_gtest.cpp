#include <gtest/gtest.h>

#include "networkit/graph/Graph.hpp"

#include "sms/auxiliary/compact_graph.hpp"

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

TEST(CompactGraph, CreateEmptyEdge) {
    auto e = WeightedEdge<int16_t>(1, 3, 10.0);
    ASSERT_EQ(e.u, 1);
    ASSERT_EQ(e.v, 3);
}

TEST(CompactGraph, CreateEmptyGraph) {
    auto g = CompactGraph<int16_t>();
    ASSERT_EQ(g.numberOfNodes(), 0);
    ASSERT_EQ(g.numberOfEdges(), 0);
}

TEST(CompactGraph, CopyNetworKit) {
    NetworKit::Graph g(5, true);
    g.addEdge(0, 1, 1);
    g.addEdge(1, 2, 1);
    g.addEdge(1, 3, 1);
    g.addEdge(3, 4, 1);
    g.addEdge(1, 4, 1);

    auto cg = CompactGraph<int16_t>(g);

    ASSERT_EQ(cg.numberOfNodes(), 5);
    ASSERT_EQ(cg.numberOfEdges(), 5);


    for (auto node: g.nodeRange()) {
        ASSERT_EQ(g.degree(node), cg.degree(node));
        auto signedNode = static_cast<int16_t>(node);
        for (auto e: cg.neighborRange(signedNode)) {
            ASSERT_TRUE(g.hasEdge(e.u, e.v));
            ASSERT_EQ(g.weight(e.u, e.v), e.weight);
        }
    }

    int c = 0;
    for (short node: cg.nodeRange()) {
        ASSERT_EQ(c++, node);
    }
    ASSERT_EQ(c - 1, 4);

    c = 0;
    for ([[maybe_unused]] auto edge: cg.edgeRange()) {
        c++;
    }
    ASSERT_EQ(c, 5);
}

TEST(CompactGraph, CopyNetworKitClique) {
    CompactGraph<int64_t> g(cliqueGraph(100));
}

TEST(CompactGraph, edgeIdRange) {
    NetworKit::Graph g(5, true, false);
    g.addEdge(0, 1, 1);
    g.addEdge(1, 2, 1);
    g.addEdge(1, 3, 1);
    g.addEdge(3, 4, 1);
    g.addEdge(4, 1, 1);

    auto cg = CompactGraph<int16_t>(g);

    auto map = cg.edgeIDmapping();

    for (auto e: cg.edgeRange()) {
        ASSERT_EQ(e.id, cg.edgeId(e.u, e.v));

        ASSERT_EQ(e.id, map[cg.edgeId(e.v, e.u)]);
    }

    for (auto u: cg.nodeRange()) {
        for (auto v: cg.neighborRange(u)) {
            ASSERT_EQ(v.id, cg.edgeId(u, v.v));
        }
    }

    auto edgeArray = cg.edgeArray();

    for (int i = 0; i < 2 * cg.numberOfEdges(); i++) {
        ASSERT_EQ(edgeArray[i].id, i);
        ASSERT_EQ(i, map[map[i]]);
    }
}