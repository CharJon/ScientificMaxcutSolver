#include <gtest/gtest.h>

#include "networkit/graph/Graph.hpp"
#include "sms/auxiliary/odd_closed_walk.hpp"


TEST(OddClosedWalk, Init) {
    OddClosedWalk ow(0);
    ow.addCrossEdge(1);
    ow.addStayEdge(2);
    ow.addCrossEdge(3);
    ow.addCrossEdge(0);
}

TEST(OddClosedWalk, isValid) {
    OddClosedWalk ow(0);
    ow.addCrossEdge(1);
    ow.addStayEdge(2);
    ow.addStayEdge(3);
    ow.addStayEdge(0);
    ow.addStayEdge(4);
    ow.addStayEdge(5);
    ow.addStayEdge(6);
    ow.addStayEdge(2);
    ow.addStayEdge(1);
    ow.addStayEdge(2);

    ASSERT_FALSE(ow.isValid());

    ow.addCrossEdge(0);

    ASSERT_FALSE(ow.isValid());

    ow.addCrossEdge(1);
    ow.addStayEdge(0);

    ASSERT_TRUE(ow.isValid());
}

TEST(OddClosedWalk, IsValidGraph) {
    NetworKit::Graph g(5);
    g.addEdge(0, 1);
    g.addEdge(1, 0);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(4, 0);
    g.addEdge(0, 3);

    OddClosedWalk ow(0);

    ow.addStayEdge(1);
    ow.addCrossEdge(2);
    ow.addStayEdge(0);

    ASSERT_TRUE(ow.isValid());
    ASSERT_FALSE(ow.isValid(g));

    OddClosedWalk ow2(0);

    ow2.addStayEdge(1);
    ow2.addStayEdge(2);
    ow2.addStayEdge(3);
    ow2.addCrossEdge(0);
    ow2.addStayEdge(4);
    ow2.addStayEdge(3);
    ow2.addStayEdge(2);
    ow2.addStayEdge(1);
    ow2.addStayEdge(0);

    ASSERT_TRUE(ow2.isValid(g));
}


TEST(OddClosedWalk, Extract) {
    OddClosedWalk ow(0);

    ow.addStayEdge(1);
    ow.addStayEdge(2);
    ow.addStayEdge(3);
    ow.addCrossEdge(0);
    ow.addStayEdge(4);
    ow.addStayEdge(3);
    ow.addStayEdge(2);
    ow.addStayEdge(1);
    ow.addStayEdge(0);

    auto ex = ow.extract(3, 6);
    ASSERT_TRUE(ex.isValid());

    ASSERT_DEBUG_DEATH(ow.extract(2, 6), "Assertion");
    ASSERT_DEBUG_DEATH(ow.extract(6, 3), "Assertion");
}


TEST(OddClosedWalk, DISABLED_splitOnChordInvalidChord) {
    OddClosedWalk ow(0);

    ow.addCrossEdge(1);
    ow.addCrossEdge(2);
    ow.addCrossEdge(3);
    ow.addCrossEdge(0);
    ow.addStayEdge(4);
    ow.addStayEdge(3);
    ow.addStayEdge(2);
    ow.addCrossEdge(1);
    ow.addStayEdge(0);

    ASSERT_DEBUG_DEATH(ow.splitOnChord(0, 3, true, false), "Assertion");
}

TEST(OddClosedWalk, splitOnChordSimple) {
    OddClosedWalk ow(0);

    ow.addCrossEdge(1);
    ow.addStayEdge(2);
    ow.addStayEdge(3);
    ow.addStayEdge(4);
    ow.addStayEdge(0);

    auto split = ow.splitOnChord(0, 3, false, true);
    ASSERT_EQ(split.getIthNode(0), 0);
    ASSERT_EQ(split.getIthNode(1), 3);
    ASSERT_EQ(split.getIthNode(2), 4);
    ASSERT_EQ(split.getIthNode(3), 0);

    ASSERT_TRUE(split.ithEdgeIsCross(0));
    ASSERT_TRUE(split.isValid());

    auto splitInner = ow.splitOnChord(0, 3, true, false);
    ASSERT_EQ(splitInner.getIthNode(0), 0);
    ASSERT_EQ(splitInner.getIthNode(1), 1);
    ASSERT_EQ(splitInner.getIthNode(2), 2);
    ASSERT_EQ(splitInner.getIthNode(3), 3);
    ASSERT_EQ(splitInner.getIthNode(4), 0);

    ASSERT_TRUE(splitInner.isValid());
}

TEST(OddClosedWalk, DISABLED_splitOnChordNoCrossEdges) {
    OddClosedWalk ow(0);

    ow.addCrossEdge(1);
    ow.addStayEdge(2);
    ow.addStayEdge(3);
    ow.addStayEdge(4);
    ow.addStayEdge(0);

    ASSERT_DEBUG_DEATH(ow.splitOnChord(0, 3, false, false), "Assertion");
}

TEST(OddClosedWalk, violatedOddSelectionClosedWalkEvenCross) {
    auto ocw = violatedOddSelectionClosedWalk({1, 2, 3, 4}, {0.95, 0.95, 0.95, 0.6});

    ASSERT_TRUE(ocw.has_value());
    ASSERT_TRUE(ocw->isValid());
    ASSERT_EQ(ocw->size(), 4);
}

TEST(OddClosedWalk, violatedOddSelectionClosedWalkEvenCrossImpossible) {
    auto ocw = violatedOddSelectionClosedWalk({1, 2, 3, 4}, {0.95, 0.95, 0.95, 0.95});

    ASSERT_FALSE(ocw.has_value());
}

TEST(OddClosedWalk, violatedOddSelectionClosedWalkStayToCross) {
    auto ocw = violatedOddSelectionClosedWalk({1, 2, 3, 4}, {0.95, 0.95, 0.1, 0.4});

    ASSERT_TRUE(ocw.has_value());
    ASSERT_TRUE(ocw->isValid());
    ASSERT_EQ(ocw->size(), 4);
}

TEST(OddClosedWalk, violatedOddSelectionClosedWalkAllStay)
{
    auto ocw = violatedOddSelectionClosedWalk({1, 2, 3, 4}, {0.1, 0.1, 0.4, 0.1});

    ASSERT_TRUE(ocw.has_value());
    ASSERT_TRUE(ocw->isValid());
    ASSERT_EQ(ocw->size(), 4);
}