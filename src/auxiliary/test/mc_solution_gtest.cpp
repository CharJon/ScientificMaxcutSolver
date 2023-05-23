#include <gtest/gtest.h>

#include "sms/auxiliary/mc_solution.hpp"

bool isSubset(std::vector<Edge> const &sub, std::vector<Edge> const &super)
{
    bool isSubset = true;
    for (Edge const &e: sub)
    {
        isSubset &= (std::find(super.begin(), super.end(), e) != super.end());
    }
    return isSubset;
}

TEST(SolutionClass, Create)
{
    auto graph = Graph(4);
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(3, 0);

    McSolution solution(&graph);
}

TEST(SolutionClass, UnfinishedInit)
{
    auto graph = Graph(4);
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(3, 0);

    McSolution solution(&graph);

    ASSERT_DEBUG_DEATH(solution.getCutValue(), "Assertion `part. <= 1' failed");
    ASSERT_DEBUG_DEATH(solution.getCutEdges(), "Assertion `part. <= 1' failed");
}

TEST(SolutionClass, Init)
{
    auto graph = Graph(4);
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(3, 0);

    McSolution solution(&graph);

    solution.allNodesTo0();
    ASSERT_EQ(solution.getCutValue(), 0);

    solution.allNodesTo1();
    ASSERT_EQ(solution.getCutValue(), 0);
}

TEST(SolutionClass, SquareUnweightedValue1)
{
    auto graph = Graph(4);
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(3, 0);

    McSolution solution(&graph);

    solution.allNodesTo0();
    solution.toPartition1(2);
    ASSERT_EQ(solution.getCutValue(), 2);
}

TEST(SolutionClass, SquareUnweightedCutEdges1)
{
    auto graph = Graph(4);
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(3, 0);

    McSolution solution(&graph);

    solution.allNodesTo0();
    solution.toPartition1(2);
    const std::vector<Edge> cutEdges = solution.getCutEdges();
    const std::vector<Edge> expectedCutEdges = {Edge(2, 3), Edge(1, 2)};
    ASSERT_EQ(cutEdges.size(), 2);
    ASSERT_TRUE(isSubset(expectedCutEdges, cutEdges));
    ASSERT_TRUE(isSubset(cutEdges, expectedCutEdges));
}

TEST(SolutionClass, SquareWeightedCut1)
{
    auto graph = Graph(4, true);
    graph.addEdge(0, 1, 7);
    graph.addEdge(1, 2, -3);
    graph.addEdge(2, 3, 5);
    graph.addEdge(3, 0, 0);

    McSolution solution(&graph);

    solution.allNodesTo0();
    solution.toPartition1(0);
    solution.toPartition1(2);
    ASSERT_EQ(solution.getCutValue(), 9);
}

TEST(SolutionClass, SquareFlip1)
{
    auto graph = Graph(4, true);
    graph.addEdge(0, 1, 7);
    graph.addEdge(1, 2, -3);
    graph.addEdge(2, 3, 5);
    graph.addEdge(3, 0, 0);

    McSolution solution(&graph);

    solution.allNodesTo0();

    auto part0nodesBefore = solution.getPartition0();
    auto part1nodesBefore = solution.getPartition1();
    ASSERT_EQ(part0nodesBefore.size(), 4);
    ASSERT_EQ(part1nodesBefore.size(), 0);

    solution.flipPartitioning();
    auto part0nodesAfter = solution.getPartition0();
    auto part1nodesAfter = solution.getPartition1();
    ASSERT_EQ(part0nodesAfter.size(), 0);
    ASSERT_EQ(part1nodesAfter.size(), 4);
}


TEST(SolutionClass, LineTest)
{
    auto graph = Graph(4);
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);

    McSolution solution(&graph);

    solution.toPartition1(1);
    solution.toPartition0(2);
    solution.toPartition1(3);
    ASSERT_EQ(solution.getCutValue(), 2);

    graph.addEdge(0, 1);
    ASSERT_DEBUG_DEATH(solution.getCutValue(), "");

    solution.toPartition1(0);
    ASSERT_EQ(solution.getCutValue(), 2);

    solution.flipPartitioning();
    ASSERT_EQ(solution.getCutValue(), 2);

    std::vector<Edge> x = {Edge(1, 2), Edge(2, 3)};
    ASSERT_TRUE(isSubset(solution.getCutEdges(), x));
}


TEST(SolutionClass, LoadTest)
{
    auto graph = Graph(4, true);
    graph.addEdge(0, 1, 7);
    graph.addEdge(1, 2, -3);
    graph.addEdge(2, 3, 5);
    graph.addEdge(3, 0, 0);

    McSolution solution(&graph, "test/data/solution_test.json");

    ASSERT_EQ(solution.getCutValue(), 9);
    ASSERT_TRUE(solution.isValid());
}

TEST(SolutionClass, LoadTestEmptyPartion0)
{
    auto graph = Graph(4, true);
    graph.addEdge(0, 1, 7);
    graph.addEdge(1, 2, -3);
    graph.addEdge(2, 3, 5);
    graph.addEdge(3, 0, 0);

    McSolution solution(&graph, "test/data/solution_test_empty_0.json");

    ASSERT_EQ(solution.getCutValue(), 0);
    ASSERT_TRUE(solution.isValid());
}

TEST(SolutionClass, LoadTestEmptyPartion1)
{
    auto graph = Graph(4, true);
    graph.addEdge(0, 1, 7);
    graph.addEdge(1, 2, -3);
    graph.addEdge(2, 3, 5);
    graph.addEdge(3, 0, 0);

    McSolution solution(&graph, "test/data/solution_test_empty_1.json");

    ASSERT_EQ(solution.getCutValue(), 0);
    ASSERT_TRUE(solution.isValid());
}

TEST(SolutionClass, LoadTestEmptyBoth)
{
    auto graph = Graph(4, true);
    graph.addEdge(0, 1, 7);
    graph.addEdge(1, 2, -3);
    graph.addEdge(2, 3, 5);
    graph.addEdge(3, 0, 0);

    McSolution solution(&graph, "test/data/solution_test_double_empty.json");

    ASSERT_FALSE(solution.isValid());
}

TEST(SolutionClass, LoadTestDoubleDef)
{
    auto graph = Graph(4, true);
    graph.addEdge(0, 1, 7);
    graph.addEdge(1, 2, -3);
    graph.addEdge(2, 3, 5);
    graph.addEdge(3, 0, 0);

    ASSERT_DEBUG_DEATH(McSolution solution(&graph, "test/data/solution_test_double_definition.json"),
                       "Assertion");
}


TEST(SolutionClass, LoadTestOutOfBounds)
{
    auto graph = Graph(3, true);
    graph.addEdge(0, 1, 7);
    graph.addEdge(1, 2, -3);

    ASSERT_DEBUG_DEATH(McSolution solution(&graph, "test/data/solution_test.json"), "Assertion `u < n' failed");
}