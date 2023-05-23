#ifndef SMS_ENUMERATION_SOLVER_HPP
#define SMS_ENUMERATION_SOLVER_HPP

#include <vector>
#include "networkit/graph/Graph.hpp"

using NetworKit::node;
using NetworKit::edgeweight;

/*
 * A class to solve max cut via full enumeration of all solutions.
 * Only works for instances with less than 32 vertices.
 */
class EnumerationSolver {
    using partition_t = uint32_t;

public:
    NetworKit::Graph const *graph;
    partition_t bestPartition = 0;
    double bestValue = 0.;

    partition_t fixedValues_ = 0;
    partition_t fixedMask_ = 0;


    explicit EnumerationSolver(const NetworKit::Graph *g) {
        assert(g->numberOfNodes() < 32);
        graph = g;
    }

    /***
     *
     * @param g Graph to solve max cut on
     * @param fixedNodes Vector of (0-indexed) node ids which should be fixed to a certain side
     * @param fixedValues Vector of fixed assignment
     */
    EnumerationSolver(const NetworKit::Graph *g, const std::vector<node> &fixedNodes,
                      const std::vector<bool> &fixedValues) {
        assert(g->numberOfNodes() < 32);
        assert(fixedNodes.size() == fixedValues.size());

        graph = g;

        fixedValues_ = 0;
        for (unsigned int i = 0; i < fixedValues.size(); i++) {
            fixedValues_ |= (fixedValues[i] << fixedNodes[i]);
        }
        fixedMask_ = 0;
        for (auto u: fixedNodes) {
            fixedMask_ |= (1UL << u);
        }
    }


    void solve();

    edgeweight getCutValue(partition_t) const;

    edgeweight getCutValueLambda(partition_t cut) const;

    edgeweight parallelGetCutValue(partition_t cut) const;

    std::vector<bool> bestPartitionVec() const;

private:
    void solveNoFixed();

    void solveFixed();

};

#endif //SMS_ENUMERATION_SOLVER_HPP
