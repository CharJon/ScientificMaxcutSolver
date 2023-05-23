#include <gtest/gtest.h>

#include "sms/auxiliary/ocw_seperator.hpp"

TEST(OcwSeparator, SimpleNoPath) {
    NetworKit::Graph g(3, true);
    g.addEdge(0, 1, 0.4);
    g.addEdge(1, 2, 0.4);
    g.addEdge(2, 0, 0.4);

    OcwSeparator ocwSeparator(&g);
    ocwSeparator.updateWeights(0, 1, 0.3);
    ocwSeparator.updateWeights(1, 2, 0.3);
    ocwSeparator.updateWeights(2, 0, 0.3);

    ocwSeparator.getViolatedOcws(0);
}

TEST(OcwSeparator, SimplePath) {
    NetworKit::Graph g(3, true);
    g.addEdge(0, 1, 0.4);
    g.addEdge(1, 2, 0.4);
    g.addEdge(2, 0, 0.4);
    g.indexEdges();

    OcwSeparator ocwSeparator(&g);
    ocwSeparator.updateWeights(0, 1, 0.9);
    ocwSeparator.updateWeights(1, 2, 0.9);
    ocwSeparator.updateWeights(2, 0, 0.9);

    auto res = ocwSeparator.getViolatedOcws(0);
    ASSERT_EQ(res.size(), 1);
    ASSERT_TRUE(res[0].isValid(g));
}

TEST(OcwSeparator, ExtractSimple1) {
    NetworKit::Graph g(4);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);
    g.addEdge(0, 3);
    g.indexEdges();

    OcwSeparator ocws(&g);
    ocws.updateWeights(0, 1, 1.);
    ocws.updateWeights(0, 2, 1.);
    ocws.updateWeights(1, 2, 1.);
    ocws.updateWeights(0, 3, 0.1);

    OddClosedWalk ocw(3);
    ocw.addEdge(0, -1);
    ocw.addEdge(1, 1);
    ocw.addEdge(2, 1);
    ocw.addEdge(0, 1);
    ocw.addEdge(3, -1);

    ocws.simpleOddCycles(ocw);
}


TEST(OcwSeparator, Extract1) {
    NetworKit::Graph g(4);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(0, 3);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.indexEdges();

    OcwSeparator ocws(&g);
    ocws.updateWeights(0, 1, 0.5);
    ocws.updateWeights(0, 2, 0.3);
    ocws.updateWeights(0, 3, 0.1);
    ocws.updateWeights(1, 2, 0.1);
    ocws.updateWeights(2, 3, 0.1);

    OddClosedWalk ocw(0);
    ocw.addEdge(1, 1);
    ocw.addEdge(2, -1);
    ocw.addEdge(3, -1);
    ocw.addEdge(0, -1);

    ocws.simpleOddCycles(ocw);

    ocws.extractCycles(ocw);
}

TEST(OcwSeparator, Extract2) {
    NetworKit::Graph g(7);
    g.addEdge(0, 1);
    g.addEdge(0, 6);
    g.addEdge(1, 2);
    g.addEdge(1, 6);
    g.addEdge(2, 3);
    g.addEdge(2, 4);
    g.addEdge(2, 5);
    g.addEdge(3, 4);
    g.addEdge(4, 5);
    g.addEdge(5, 6);
    g.indexEdges();

    OcwSeparator ocws(&g);
    ocws.updateWeights(0, 1, 0.1);
    ocws.updateWeights(0, 6, 0.1);
    ocws.updateWeights(1, 2, 0.9);
    ocws.updateWeights(1, 6, 0.5);
    ocws.updateWeights(2, 3, 0.9);
    ocws.updateWeights(2, 4, 0.1);
    ocws.updateWeights(2, 5, 0.1);
    ocws.updateWeights(3, 4, 0.1);
    ocws.updateWeights(4, 5, 0.1);
    ocws.updateWeights(5, 6, 0.9);

    OddClosedWalk ocw(0);
    ocw.addEdge(1, -1);
    ocw.addEdge(2, 1);
    ocw.addEdge(3, 1);
    ocw.addEdge(4, -1);
    ocw.addEdge(5, -1);
    ocw.addEdge(6, 1);
    ocw.addEdge(0, -1);

    ocws.simpleOddCycles(ocw);

    ocws.extractCycles(ocw);
}
