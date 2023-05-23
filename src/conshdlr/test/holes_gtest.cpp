#include <gtest/gtest.h>

#include "sms/conshdlr/holes.hpp"

TEST(HoleSeparator, noFourHoles) {
    NetworKit::Graph g(4, true);
    g.addEdge(0, 1, 0.9);
    g.addEdge(1, 2, 0.3);
    g.addEdge(2, 3, 0.2);
    g.addEdge(3, 0, 0.1);

    auto ts = HoleSeparator(&g);
    ts.updateWeights(0, 1, 0.3);
    ts.updateWeights(1, 2, 0.3);
    ts.updateWeights(2, 3, 0.2);
    ts.updateWeights(3, 0, 0.1);

    auto res = ts.getViolatedFourHoles();
    ASSERT_EQ(res.size(), 0);
}

TEST(HoleSeparator, oneFourHoles) {
    NetworKit::Graph g(4, true);
    g.addEdge(0, 1, 0.9);
    g.addEdge(1, 2, 0.3);
    g.addEdge(2, 3, 0.2);
    g.addEdge(3, 0, 0.1);

    auto ts = HoleSeparator(&g);
    ts.addFourHole(0, 1, 2, 3);
    ts.updateWeights(0, 1, 0.9);
    ts.updateWeights(1, 2, 0.3);
    ts.updateWeights(2, 3, 0.2);
    ts.updateWeights(3, 0, 0.1);

    auto res = ts.getViolatedFourHoles();
    ASSERT_EQ(res.size(), 1);
    res[0].isValid(g);
}