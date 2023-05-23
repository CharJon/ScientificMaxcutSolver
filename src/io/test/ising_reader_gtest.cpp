#include <gtest/gtest.h>

#include "sms/io/ising_reader.hpp"


TEST(SGParser, SimpleParsing) {
    sgParser parser("test/data/square.sg");

    auto inst = parser.getInstance();
    ASSERT_EQ(inst.getNumberOfSpins(), 4);
    ASSERT_EQ(inst.getNumberOfInteractions(), 4);
    ASSERT_EQ(inst.getSolutionValue({1, 1, 1, 1}), 7);
    ASSERT_EQ(inst.getSolutionValue({1, -1, 1, 1}), -3);
    ASSERT_EQ(inst.getSolutionValue({1, -1, -1, 1}), 25);
}

TEST(GSGParser, SimpleParsing) {
    gsgParser parser("test/data/simpleGrid.gsg");

    auto inst = parser.getInstance();
    ASSERT_EQ(inst.getNumberOfSpins(), 9);
    ASSERT_EQ(inst.getNumberOfInteractions(), 18);


}

TEST(GSGParser, 3DParsing) {
    gsgParser parser("test/data/t3g7_6666.gsg");

    auto inst = parser.getInstance();
    ASSERT_EQ(inst.getNumberOfSpins(), 343);
    ASSERT_EQ(inst.getNumberOfInteractions(), 1029);
}

TEST(GSGParser, WrongGrid) {
    ASSERT_ANY_THROW(gsgParser parser("test/data/error_grid_undetermined.gsg"));
}

TEST(GSGParser, MissingEdge) {
    ASSERT_ANY_THROW(gsgParser parser("test/data/error_missing_edge.gsg"));
}

TEST(GSGParser, TooFewEdges) {
    ASSERT_ANY_THROW(gsgParser parser("test/data/error_wrong_number_of_edges.gsg"));
}

TEST(GSGParser, TooManyEdges) {
    ASSERT_ANY_THROW(gsgParser parser("test/data/error_wrong_number_of_edges_2.gsg"));
}