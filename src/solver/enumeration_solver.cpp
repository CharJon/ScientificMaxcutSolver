#include "sms/solver/enumeration_solver.hpp"

#include <vector>
#include "networkit/graph/Graph.hpp"

using NetworKit::node;


void EnumerationSolver::solve() {
    if (fixedMask_ == 0) {
        solveNoFixed();
    } else {
        solveFixed();
    }
}


void EnumerationSolver::solveNoFixed() {
    assert(fixedMask_ == 0);

    partition_t currentPartition_ = 0UL;

    partition_t best = currentPartition_;
    double currentBestValue = getCutValue(currentPartition_);


    unsigned long upperBoundSolutionVector = 1UL << (graph->numberOfNodes() - 1);
    while (currentPartition_ < upperBoundSolutionVector) {
        double currentValue = getCutValueLambda(currentPartition_);

        if (currentValue > currentBestValue) {
            best = currentPartition_;
            currentBestValue = currentValue;
        }

        currentPartition_ += 1;
    }

    bestPartition = best;
    bestValue = currentBestValue;
}


void EnumerationSolver::solveFixed() {
    assert(fixedMask_ != 0);

    partition_t currentPartition_ = fixedValues_;

    partition_t best = currentPartition_;
    double currentBestValue = getCutValue(currentPartition_);

    currentPartition_ |= fixedMask_; // set all fixed to one
    currentPartition_ += 1;

    unsigned long upperBoundSolutionVector = 1UL << graph->numberOfNodes();
    while (currentPartition_ < upperBoundSolutionVector) {
        currentPartition_ &= (~fixedMask_); // reset fixed
        currentPartition_ |= (fixedValues_); // set fixed to correct values

        double currentValue = getCutValueLambda(currentPartition_);

        if (currentValue > currentBestValue) {
            best = currentPartition_;
            currentBestValue = currentValue;
        }

        currentPartition_ |= fixedMask_; // set all fixed to one
        currentPartition_ += 1;
    }

    bestPartition = best;
    bestValue = currentBestValue;
}

template<typename T>
inline bool inDifferentPartitions(T cut, unsigned int pos1, unsigned int pos2) {
    return ((cut >> pos1) ^ (cut >> pos2)) & (1UL);
}

double EnumerationSolver::getCutValue(partition_t cut) const {
    double cutValue = 0;

    for (auto e: graph->edgeWeightRange()) {
        cutValue += inDifferentPartitions(cut, e.u, e.v) ? e.weight : 0;
    }

    return cutValue;
}

inline double EnumerationSolver::getCutValueLambda(partition_t cut) const {
    double cutValue = 0;

    graph->forEdges([&cutValue, &cut](node u, node v, NetworKit::edgeweight weight) {
        cutValue += inDifferentPartitions(cut, u, v) * weight;
    });

    return cutValue;
}

NetworKit::edgeweight
EnumerationSolver::parallelGetCutValue(partition_t cut) const {

    auto cutV = graph->parallelSumForEdges([cut](node u, node v, NetworKit::edgeweight w) {
        //return !!(getBit(cut, u) ^ getBit(cut, v)) ? w : 0;
        return inDifferentPartitions(cut, u, v) * w;
    });

    return cutV;
}

std::vector<bool> EnumerationSolver::bestPartitionVec() const {
    auto sol = std::vector<bool>(graph->numberOfNodes(), false);
    for (auto u: graph->nodeRange()) {
        sol[u] = (bestPartition >> u) & 1UL;
    }
    return sol;
}

