#include <gtest/gtest.h>

#include "sms/instance/maxcut.hpp"

TEST(MaxCut, createClass1) {
    NetworKit::Graph g(0);
    MaxCut maxcut(g);
    ASSERT_EQ(maxcut.getNumberOfNodes(), 0);
    ASSERT_EQ(maxcut.getNumberOfEdges(), 0);
    ASSERT_EQ(maxcut.getScalingFactor(), 1);
    ASSERT_EQ(maxcut.getOffset(), 0);
}

TEST(MaxCut, createClass2) {
    NetworKit::Graph g(3);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 0);

    MaxCut maxcut(g);
    ASSERT_EQ(maxcut.getNumberOfNodes(), 3);
    ASSERT_EQ(maxcut.getNumberOfEdges(), 3);
    ASSERT_EQ(maxcut.getScalingFactor(), 1);
    ASSERT_EQ(maxcut.getOffset(), 0);
}


