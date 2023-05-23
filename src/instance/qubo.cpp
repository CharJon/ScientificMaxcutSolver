#include "sms/instance/qubo.hpp"

#include <cassert>
#include <algorithm>
#include "sms/auxiliary/math.hpp"


QUBO::QUBO(int dim) {
    dim_ = dim;
    matrix_ = new double[dim * dim];
    resetToZero();

    free_ = true;

    scalingFactor_ = 1;
    offset_ = 0;
}

QUBO::QUBO(double *q, int dim) {
    dim_ = dim;
    matrix_ = q;

    free_ = false;

    scalingFactor_ = 1;
    offset_ = 0;
}

QUBO::~QUBO() {
    if (free_)
        delete[] matrix_;
}

void QUBO::setValue(int i, int j, double value) {
    assert(i < dim_ && j < dim_);
    matrix_[i * dim_ + j] = value;
}

void QUBO::setValueU(unsigned int i, unsigned int j, double value) {
    assert(i < dim_ && j < dim_);
    matrix_[i * dim_ + j] = value;
}

double QUBO::getValue(int i, int j) const {
    assert(i < dim_ && j < dim_);
    return matrix_[i * dim_ + j];
}


double QUBO::getSolutionValueBLAS(const double *solVector) const {
    return quboEvaluation(matrix_, solVector, dim_, scalingFactor_) + offset_;
}

int QUBO::getDim() const {
    return dim_;
}

bool QUBO::isValid() const {
    return matrix_ != nullptr;
}

void QUBO::resetToZero() {
    fillAll(0);
}

void QUBO::fillAll(double value) {
    std::fill_n(matrix_, dim_ * dim_, value);
}
