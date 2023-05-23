#include <gtest/gtest.h>

#include "networkit/graph/Graph.hpp"
#include "networkit/graph/GraphTools.hpp"
#include "networkit/io/EdgeListReader.hpp"
#include "sms/auxiliary/graphs.hpp"
#include "sms/io/io.hpp"


void test_subgraph(NetworKit::Graph &g, std::vector<NetworKit::node> &nodes, bool nk = false) {
    auto s = nk ? getInducedSubgraphNetworkit(g, nodes) : getInducedSubgraph(g, nodes);

    uint c = 0;
    for (auto i: nodes) {
        ASSERT_EQ(s.orig2sub[i], c);
        c++;
        ASSERT_EQ(i, s.sub2orig[s.orig2sub[i]]);
    }

    for (uint i = 0; i < nodes.size(); i++) {
        ASSERT_EQ(s.sub2orig[i], nodes[i]);
    }

    for (auto e: s.subgraph.edgeWeightRange())
        ASSERT_EQ(e.weight, g.weight(s.sub2orig[e.u], s.sub2orig[e.v]));

    auto nk_sub = NetworKit::GraphTools::subgraphFromNodes(g, nodes.begin(), nodes.end(), true);

    ASSERT_EQ(nk_sub.numberOfNodes(), s.subgraph.numberOfNodes());
    ASSERT_EQ(nk_sub.numberOfEdges(), s.subgraph.numberOfEdges());
    ASSERT_EQ(nk_sub.totalEdgeWeight(), s.subgraph.totalEdgeWeight());
}


TEST(InducedSubgraph, SimpleTest) {
    NetworKit::Graph g(4);

    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 3);
    g.addEdge(2, 3);

    std::vector<NetworKit::node> nodes = {0, 1};
    auto s = getInducedSubgraph(g, nodes);

    s.subgraph.hasEdge(0, 1);


    uint c = 0;
    for (auto i: nodes) {
        ASSERT_EQ(s.orig2sub[i], c);
        c++;
        ASSERT_EQ(i, s.sub2orig[s.orig2sub[i]]);
    }

    for (uint i = 0; i < nodes.size(); i++) {
        ASSERT_EQ(s.sub2orig[i], nodes[i]);
    }


}

TEST(InducedSubgraph, ComplexTest) {
    NetworKit::Graph g = mcFileToGraph("test/data/ego-facebook.mc");

    count subgraphSize = g.numberOfNodes() * 2 / 3;
    std::vector<NetworKit::node> nodes;

    for (uint i = 0; i < subgraphSize; i++) {
        nodes.emplace_back(i);
    }

    test_subgraph(g, nodes);
}

TEST(InducedSubgraph, ComplexTest2) {
    NetworKit::Graph g = mcFileToGraph("test/data/subgraph_test.mc");

    std::vector<NetworKit::node> nodes = {0, 1, 2, 3};

    test_subgraph(g, nodes);
}

TEST(InducedSubgraph, ComplexTest3) {
    NetworKit::EdgeListReader el(' ', 0);
    NetworKit::Graph g = el.read("test/data/ia-wiki-Talk.el");

    count subgraphSize = g.numberOfNodes() * 2 / 3;
    std::vector<NetworKit::node> nodes(subgraphSize);

    for (uint i = 0; i < subgraphSize; i++) {
        nodes[i] = i;
    }

    test_subgraph(g, nodes);
}

TEST(InducedSubgraph, ComplexTest3Nk) {
    NetworKit::EdgeListReader el(' ', 0);
    NetworKit::Graph g = el.read("test/data/ia-wiki-Talk.el");

    count subgraphSize = g.numberOfNodes() * 2 / 3;
    std::vector<NetworKit::node> nodes(subgraphSize);

    for (uint i = 0; i < subgraphSize; i++) {
        nodes[i] = i;
    }

    test_subgraph(g, nodes, true);
}

TEST(getCutValue, SimpleTest) {
    NetworKit::Graph g = mcFileToGraph("test/data/square.mc");

    ASSERT_EQ(getCutValue(g, {0, 0, 0, 1, 1}), -3.2);
    ASSERT_EQ(getCutValue(g, {0, 0, 0, 1, 0}), -1.4);
    ASSERT_EQ(getCutValue(g, {0, 0, 0, 0, 0}), 0);
    ASSERT_EQ(getCutValue(g, {0, 1, 1, 1, 1}), 0);
}

TEST(printGraphInformation, ego_facebook) {
    NetworKit::Graph g = mcFileToGraph("test/data/ego-facebook.mc");
    printGraphInformation(g);
}