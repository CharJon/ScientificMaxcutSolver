#include <gtest/gtest.h>

#include "sms/auxiliary/math.hpp"

TEST(BLAS, VectorMultMatrix) {
    double A[3][3] = {{1, 2, 3},
                      {4, 5, 6},
                      {7, 8, 9}};
    double x[3] = {1, 1, 1};
    double y[3] = {0, 0, 0};

    // Perform matrix-vector multiplication
    cblas_dgemv(CblasRowMajor, CblasNoTrans, 3, 3, 1.0, &A[0][0], 3, x, 1, 0.0, y, 1);

    ASSERT_EQ(y[0], 6);
    ASSERT_EQ(y[1], 15);
    ASSERT_EQ(y[2], 24);
}

TEST(BLAS, QUBO) {
    double A[3][3] = {{1, 2, 3},
                      {4, 5, 6},
                      {7, 8, 9}};
    double x[3] = {1, 0, 1};
    double y[3] = {0, 0, 0};

    // Perform matrix-vector multiplication
    cblas_dgemv(CblasRowMajor, CblasNoTrans, 3, 3, 1.0, &A[0][0], 3, x, 1, 0.0, y, 1);
    auto res = cblas_ddot(3, x, 1, y, 1);

    ASSERT_EQ(res, 20);
}

TEST(BLAS, QUBO_2) {
    double A[2][2] = {{-1, -1},
                      {-1, -1}};
    double x[3] = {1, 1}; // JC: right side was {1, 1} instead of {1, 1, 1}
    double y[3] = {0, 0}; // JC: right side was {0, 0}

    // Perform matrix-vector multiplication
    cblas_dgemv(CblasRowMajor, CblasNoTrans, 2, 2, 1.0, &A[0][0], 2, x, 1, 0.0, y, 1);
    auto res = cblas_ddot(2, x, 1, y, 1);

    ASSERT_EQ(res, -4);
}
