#ifndef SMS_MATH_HPP
#define SMS_MATH_HPP

#include "cblas.h"


/**
 * @brief Computes alpha * x^T * Q * x
 * @param Q Q matrix
 * @param x x vector
 * @param scalingFactor alpha
 * @return x^T * Q * x
 */
inline double quboEvaluation(const double *Q, const double *x, int dim, double scalingFactor) {
    auto *y = new double[dim];
    cblas_dgemv(CblasRowMajor, CblasNoTrans, dim, dim, scalingFactor, Q, dim, x, 1.0, 0.0, y, 1.0);
    double res = cblas_ddot(dim, x, 1, y, 1);
    delete[] y;
    return res;
}

/**
 * @brief Computes x^T * Q * x
 * @param Q Q matrix
 * @param x x vector
 * @return x^T * Q * x
 */
inline double quboEvaluation(const double *Q, const double *x, int dim) {
    return quboEvaluation(Q, x, dim, 1.0);
}


#endif //SMS_MATH_HPP
