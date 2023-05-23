#include <gtest/gtest.h>

#include "sms/auxiliary/biconnected_partition.hpp"
#include "sms/io/io.hpp"

TEST(BicconectedComps, BasicTest) {
    NetworKit::Graph g = mcFileToGraph("test/data/pm1s_50.1.mc");

    BiconnectedPartition biconn(g);
    biconn.compute_components();

    ASSERT_GT(biconn.components.size(), 0);
    ASSERT_GT(biconn.bridges.size(), 0);
}

TEST(BicconectedComps, SimpleGraph) {
    NetworKit::Graph g(5);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(0, 3);
    g.addEdge(0, 4);
    g.addEdge(3, 4);
    g.addEdge(1, 2);

    BiconnectedPartition biconn(g);
    biconn.compute_components();

    ASSERT_EQ(biconn.components.size(), 2);
    ASSERT_EQ(biconn.bridges.size(), 0);

    ASSERT_EQ(biconn.tree.numberOfNodes(), 3);
    ASSERT_EQ(biconn.tree.numberOfEdges(), 2);
    ASSERT_EQ(biconn.tree_map[0].size(), 3);

    ASSERT_EQ(biconn.inverse_tree_map[0], 2);
}

TEST(BicconectedComps, GraphWithBridge) {
    NetworKit::Graph g(6);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);

    g.addEdge(0, 5);

    g.addEdge(5, 4);
    g.addEdge(3, 4);
    g.addEdge(3, 5);

    BiconnectedPartition biconn(g);
    biconn.compute_components();

    ASSERT_EQ(biconn.components.size(), 3);
    ASSERT_EQ(biconn.bridges.size(), 1);

    ASSERT_EQ(biconn.tree.numberOfNodes(), 5);
    ASSERT_EQ(biconn.tree.numberOfEdges(), 4);
    ASSERT_EQ(biconn.tree_map[0].size(), 3);

    ASSERT_EQ(biconn.inverse_tree_map[0], 3);
    ASSERT_EQ(biconn.inverse_tree_map[5], 4);
}

TEST(BicconectedComps, StarGraph) {
    NetworKit::Graph g = mcFileToGraph("test/data/star.mc");

    BiconnectedPartition biconn(g);
    biconn.compute_components();

    ASSERT_EQ(biconn.components.size(), 20);
    ASSERT_EQ(biconn.bridges.size(), 20);

    ASSERT_EQ(biconn.tree.numberOfNodes(), 21);
    ASSERT_EQ(biconn.tree.numberOfEdges(), 20);
    ASSERT_EQ(biconn.tree_map[0].size(), 2);

    ASSERT_EQ(biconn.inverse_tree_map[1], 20);
    ASSERT_EQ(biconn.tree_map[20].size(), 1);
    ASSERT_EQ(biconn.tree_map[20][1], 0);
}

TEST(BicconectedComps, DoubleStarGraph) {
    NetworKit::Graph g = mcFileToGraph("test/data/double_star.mc");

    BiconnectedPartition biconn(g);
    biconn.compute_components();

    ASSERT_EQ(biconn.components.size(), 40);
    ASSERT_EQ(biconn.bridges.size(), 40);

    ASSERT_EQ(biconn.tree.numberOfNodes(), 61);
    ASSERT_EQ(biconn.tree.numberOfEdges(), 60);
    ASSERT_EQ(biconn.tree_map[5].size(), 2);

    ASSERT_EQ(biconn.inverse_tree_map[1], 40);
    ASSERT_EQ(biconn.tree_map[40].size(), 1);
    ASSERT_EQ(biconn.tree_map[40][0], 1);

    ASSERT_EQ(biconn.inverse_tree_map[1], 40);
    ASSERT_EQ(biconn.tree_map[41].size(), 1);
    ASSERT_EQ(biconn.tree_map[41][1], 0);
}