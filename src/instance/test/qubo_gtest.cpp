#include <gtest/gtest.h>

#include "sms/instance/qubo.hpp"

TEST(QuBO, createClass1) {
    QUBO qubo(5);
    ASSERT_TRUE(qubo.isValid());
    ASSERT_EQ(qubo.getDim(), 5);
}

TEST(QuBO, createClass2) {
    QUBO qubo(5);
    ASSERT_TRUE(qubo.isValid());
    ASSERT_EQ(qubo.getDim(), 5);

    qubo.setValue(1, 1, 10.0);
    ASSERT_EQ(qubo.getValue(1, 1), 10.0);
}

TEST(QuBO, resetToZero1) {
    QUBO qubo(100);
    ASSERT_TRUE(qubo.isValid());
    ASSERT_EQ(qubo.getDim(), 100);

    qubo.setValue(1, 1, 7.3);
    ASSERT_EQ(qubo.getValue(1, 1), 7.3);

    qubo.resetToZero();
    for (int i = 0; i < qubo.getDim(); i++) {
        for (int j = 0; j < qubo.getDim(); j++) {
            ASSERT_EQ(qubo.getValue(i, j), 0.0);
        }
    }
}

TEST(QuBO, quboEval1) {
    QUBO qubo(100);
    ASSERT_TRUE(qubo.isValid());
    ASSERT_EQ(qubo.getDim(), 100);

    qubo.resetToZero();

    double solVector[100];
    std::fill_n(solVector, 100, 1.0);

    ASSERT_EQ(qubo.getSolutionValue(solVector), 0.0);
}

TEST(QuBO, quboEvalBLAS1) {
    QUBO qubo(100);
    ASSERT_TRUE(qubo.isValid());
    ASSERT_EQ(qubo.getDim(), 100);

    qubo.resetToZero();

    double solVector[100];
    std::fill_n(solVector, 100, 1.0);

    ASSERT_EQ(qubo.getSolutionValueBLAS(solVector), 0.0);
}