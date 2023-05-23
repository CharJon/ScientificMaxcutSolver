#ifndef SMS_BASIC_QUBO_ENUMERATION_SOLVER_HPP
#define SMS_BASIC_QUBO_ENUMERATION_SOLVER_HPP

#include <vector>
#include <limits>
#include <cassert>

#include <cblas.h>

#include "sms/auxiliary/math.hpp"
#include "sms/instance/qubo.hpp"

/*
 * JC: ToDo
 */
class BasicQuboEnumerationSolver {

public:
    explicit BasicQuboEnumerationSolver(const QUBO *q) {
        qubo_ = q;
        bestSolution_ = std::vector<int>(qubo_->getDim(), 0);
    }

    void solve();

    std::vector<int> bestSolutionVec() const;

    double bestSolutionValue() const;

    bool hasRun() const { return hasRun_; }

private:
    QUBO const *qubo_;
    std::vector<int> bestSolution_;
    double bestSolutionValue_ = std::numeric_limits<double>::max();
    bool hasRun_ = false;
};

#endif //SMS_BASIC_QUBO_ENUMERATION_SOLVER_HPP
