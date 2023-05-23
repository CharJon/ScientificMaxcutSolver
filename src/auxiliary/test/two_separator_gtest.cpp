#include <gtest/gtest.h>

#include "networkit/graph/Graph.hpp"
#include "sms/auxiliary/two_separator.hpp"
#include "sms/io/io.hpp"

TEST(TwoSeparator, BasicSquareTest)
{
    Graph g(4);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 0);

    TwoSeparator sep(g);

    sep.run();

    ASSERT_TRUE(sep.hasRun());

    ASSERT_EQ(sep.separators.size(), 2);

    ASSERT_EQ(sep.separators[0], std::make_tuple(0, 2));
    ASSERT_EQ(sep.separators[1], std::make_tuple(1, 3));
}

TEST(TwoSeparator, MediumSizeTest)
{
    Graph g(10);
    for (uint v = 0; v < 5; v++)
    {
        for (uint u = 0; u < 5; u++)
        {
            if (u < v)
            {
                g.addEdge(v, u);
                g.addEdge(v + 5, u + 5);
            }
        }
    }

    g.addEdge(0, 6);
    g.addEdge(1, 5);

    ASSERT_EQ(g.numberOfEdges(), 22);

    TwoSeparator sep(g);

    sep.run();

    ASSERT_TRUE(sep.hasRun());

    ASSERT_EQ(sep.separators.size(), 4);

    ASSERT_EQ(sep.separators[0], std::make_tuple(0, 1));
    ASSERT_EQ(sep.separators[1], std::make_tuple(0, 5));
    ASSERT_EQ(sep.separators[2], std::make_tuple(1, 6));
    ASSERT_EQ(sep.separators[3], std::make_tuple(5, 6));
}

TEST(TwoSeparator, ThreeComponents)
{
    Graph g(17);
    for (uint v = 0; v < 5; v++)
    {
        for (uint u = 0; u < 5; u++)
        {
            if (u < v)
            {
                g.addEdge(v, u);
                g.addEdge(v + 5, u + 5);
                g.addEdge(v + 10, u + 10);
            }
        }
    }

    g.addEdge(15, 0);
    g.addEdge(15, 5);
    g.addEdge(15, 10);
    g.addEdge(15, 1);
    g.addEdge(15, 6);
    g.addEdge(15, 11);

    g.addEdge(16, 0);
    g.addEdge(16, 5);
    g.addEdge(16, 10);
    g.addEdge(16, 4);
    g.addEdge(16, 9);
    g.addEdge(16, 14);

    ASSERT_EQ(g.numberOfEdges(), 42);

    TwoSeparator sep(g);

    sep.run();

    ASSERT_TRUE(sep.hasRun());

    ASSERT_EQ(sep.separators.size(), 1);

    ASSERT_EQ(sep.separators[0], std::make_tuple(15, 16));
}