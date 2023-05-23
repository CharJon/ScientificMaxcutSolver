#include "sms/solver/basic_qubo_enumeration_solver.hpp"

#include <cstring>

void BasicQuboEnumerationSolver::solve() {
    auto dim = qubo_->getDim();
    auto *current = new double[dim];
    auto *best = new double[dim];
    const auto arraySize = sizeof(double) * dim;

    std::memset(current, 0, arraySize);

    int maxVal = 1 << dim;

    for (int i = 0; i < maxVal; i++) {
        for (int j = 0; j < dim; j++) {
            current[j] = static_cast<double>((i >> j) & 1);
        }

        // double value = qubo_->getSolutionValue(current);
        double value = qubo_->getSolutionValueBLAS(current);
        if (value < bestSolutionValue_) {
            bestSolutionValue_ = value;

            std::memcpy(best, current, arraySize);
        }
    }

    for (int j = 0; j < dim; j++) {
        bestSolution_[j] = static_cast<int>(best[j]);
    }

    delete[] current;
    delete[] best;

    hasRun_ = true;
}

std::vector<int> BasicQuboEnumerationSolver::bestSolutionVec() const {
    assert(hasRun());
    return bestSolution_;
}

double BasicQuboEnumerationSolver::bestSolutionValue() const {
    assert(hasRun());
    return bestSolutionValue_;
}
