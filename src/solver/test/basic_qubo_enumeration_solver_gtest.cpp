#include <gtest/gtest.h>
#include "sms/solver/basic_qubo_enumeration_solver.hpp"

#include <vector>
#include <chrono>

#include "sms/instance/qubo.hpp"


TEST(EnumerationQubo, BasicTest) {
    const int dim = 2;
    double Q[dim][dim] = {{-1, -1},
                          {-1, -1}};

    QUBO q(&Q[0][0], dim);
    BasicQuboEnumerationSolver solver(&q);
    solver.solve();

    ASSERT_EQ(solver.bestSolutionValue(), -4);
    ASSERT_TRUE(solver.hasRun());
    auto resVec = solver.bestSolutionVec();
    for (int i = 0; i < dim; i++) {
        ASSERT_EQ(resVec[i], 1);
    }
}

TEST(EnumerationQubo, BasicTest2) {
    const int dim = 2;
    double Q[dim][dim] = {{-1, 1},
                          {-1, -1}};

    QUBO q(&Q[0][0], dim);
    BasicQuboEnumerationSolver solver(&q);
    solver.solve();

    ASSERT_EQ(solver.bestSolutionValue(), -2);
    ASSERT_TRUE(solver.hasRun());
    auto resVec = solver.bestSolutionVec();
    for (int i = 0; i < dim; i++) {
        ASSERT_EQ(resVec[i], 1);
    }
}

TEST(EnumerationQubo, NotTrivialTest) {
    const int dim = 2;
    double Q[dim][dim] = {{-1, 1},
                          {1,  -1}};

    QUBO q(&Q[0][0], dim);
    BasicQuboEnumerationSolver solver(&q);
    solver.solve();

    ASSERT_EQ(solver.bestSolutionValue(), -1);
    ASSERT_TRUE(solver.hasRun());
    auto resVec = solver.bestSolutionVec();
    ASSERT_EQ(resVec[0], 1);
    ASSERT_EQ(resVec[1], 0);
}

TEST(EnumerationQubo, 3x3) {
    const int dim = 3;
    double Q[dim][dim] = {{2.3,  1.7, 0.9},
                          {0.3,  -1,  -2.3},
                          {-0.7, 1,   -1}};

    QUBO q(&Q[0][0], dim);
    BasicQuboEnumerationSolver solver(&q);
    solver.solve();

    ASSERT_EQ(solver.bestSolutionValue(), -3.3);
    ASSERT_TRUE(solver.hasRun());
    auto resVec = solver.bestSolutionVec();
    ASSERT_EQ(resVec[0], 0);
    ASSERT_EQ(resVec[1], 1);
    ASSERT_EQ(resVec[2], 1);
}

class QuboParameterizedTestFixture : public ::testing::TestWithParam<int> {
};

TEST_P(QuboParameterizedTestFixture, MinusOneOnly) {
    int dim = GetParam();
    int numEntries = dim * dim;
    auto Q = new double[numEntries];
    for (int i = 0; i < numEntries; i++) {
        Q[i] = -1;
    }
    QUBO q(Q, dim);
    BasicQuboEnumerationSolver solver(&q);
    solver.solve();

    ASSERT_EQ(solver.bestSolutionValue(), -numEntries);
    delete[] Q;
}

INSTANTIATE_TEST_SUITE_P(AllSizes,
                         QuboParameterizedTestFixture,
                         ::testing::Values(2, 5, 10, 15, 16, 17, 18));
