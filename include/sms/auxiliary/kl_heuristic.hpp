#ifndef SMS_KL_HEURISTIC_HPP
#define SMS_KL_HEURISTIC_HPP

#include <vector>
#include <chrono>

#include "networkit/graph/Graph.hpp"

#include "sms/auxiliary/mc_solution.hpp"
#include "sms/auxiliary/graphs.hpp"

using NetworKit::node;
using NetworKit::edgeweight;
using NetworKit::Edge;
using NetworKit::Graph;

typedef struct {
    uint a;
    uint b;
    edgeweight gain;
} gainPair;

typedef struct {
    std::vector<node> A;
    std::vector<node> B;
    std::vector<bool> active;
} KLPartition;

class KLHeuristic {
public:
    Graph const *const originalGraph_;

    explicit KLHeuristic(Graph const *const originalGraph, long maxTime = 2000000)
            : originalGraph_(originalGraph), solution_(originalGraph), phase2solution_(originalGraph) {
        n_ = originalGraph_->numberOfNodes();
        uId_ = originalGraph_->upperNodeIdBound();
        maxRuntime_ = maxTime;
        phase1Opt_ = false;
        phase2Opt_ = false;
        phase1GPs_ = {};
        phase1improvements_ = {};
        phase1ks_ = {};
        phase1Iters_ = 0;
    }

    void phase1Optimization(); //all to one partition plus n dummy nodes, use for root heuristic

    void phase1Optimization(McSolution &); //optimise on a given solution

    void phase2Optimization(); //optimize without solution and reoptimize anything found

    McSolution getPrimalSolution();


    [[nodiscard]] bool phase1Opt() const;

    [[nodiscard]] bool phase2Opt() const;

    void printStats();

private:
    McSolution solution_;
    McSolution phase2solution_;
    uint64_t n_;
    uint64_t uId_;
    std::chrono::steady_clock::time_point start_;
    long maxRuntime_;
    long elapsed_;
    bool phase1Opt_;
    bool phase2Opt_;
    uint phase1Iters_;
    std::vector<uint> phase1GPs_;
    std::vector<edgeweight> phase1improvements_;
    std::vector<uint64_t> phase1ks_;
    uint phase2Iters0_{}, phase2Iters1_{};

    void repartitionOptimization(); //repartition the solution partitions [[later]]

    void phase1OptimizationSubRoutine(KLPartition &partition, uint maxGP);  // start from given partition

    [[nodiscard]] edgeweight getCost(node i, node j) const;

    [[nodiscard]] std::vector<edgeweight> computeDValues(KLPartition) const; //compute dValues for all nodes

    [[nodiscard]] gainPair findGainPair(std::vector<edgeweight> dValues, KLPartition) const;

    static std::tuple<uint64_t, edgeweight> maxGainSumIndex(std::vector<gainPair> gainPairs);

    [[nodiscard]] bool belowMaxRuntime() const;

    void done();

    [[nodiscard]] long getTimeElapsed() const;

    [[nodiscard]] long getTotalRuntime() const;

    [[nodiscard]] uint getPhase1Iters() const;
};

#endif //SMS_KL_HEURISTIC_HPP
