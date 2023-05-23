#include <gtest/gtest.h>

#include "sms/io/dimacs_reader.hpp"

TEST(DimacsReader, torusg3_8) {
    DimacsReader parser("test/data/torusg3-8.dat", 0.00001, true);

    auto i = parser.getInstance();

    //EXPECT_EQ(i.getNumberOfNodes(), 512);
    //EXPECT_EQ(i.getNumberOfEdges(), 1536);
}