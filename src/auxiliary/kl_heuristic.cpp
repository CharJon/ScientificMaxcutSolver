#include "sms/auxiliary/kl_heuristic.hpp"
#include <chrono>


McSolution KLHeuristic::getPrimalSolution() {
    if (phase1Opt_ && !phase2Opt_) {
        return solution_;
    } else if (phase2Opt_) {
        return solution_.getCutValue() > phase2solution_.getCutValue() ? solution_ : phase2solution_;
    }
    return solution_;
}

void KLHeuristic::phase1Optimization() {
    start_ = std::chrono::steady_clock::now();

    KLPartition partition;
    partition.A = std::vector<node>(n_);
    partition.B = std::vector<node>(n_);
    partition.active = std::vector<bool>(2 * n_, true);

    for (node i = 0; i < n_; i++) {    // all real nodes to partition 0
        partition.A[i] = i;
        partition.B[i] = n_ + i;
    }

    phase1OptimizationSubRoutine(partition, n_);

    for (auto i: partition.A) {
        if (i < uId_)
            solution_.toPartition0(i);
    }

    for (auto i: partition.B) {
        if (i < uId_)
            solution_.toPartition1(i);
    }
    assert(solution_.isValid());
    phase1Opt_ = true;
    done();
}

void KLHeuristic::phase1Optimization(McSolution &initSol) {
    start_ = std::chrono::steady_clock::now();

    KLPartition partition;
    partition.A = initSol.getPartition0();
    partition.B = initSol.getPartition1();
    partition.active = std::vector<bool>(n_, true);

    phase1OptimizationSubRoutine(partition, std::min(partition.A.size(), partition.B.size()));

    for (auto i: partition.A) {
        if (i < uId_)
            solution_.toPartition0(i);
    }

    for (auto i: partition.B) {
        if (i < uId_)
            solution_.toPartition1(i);
    }
    assert(solution_.isValid());
    phase1Opt_ = true;
}

void KLHeuristic::phase1OptimizationSubRoutine(KLPartition &partition, uint maxGP) {
    bool improvement = true;

    int c = 0;

    while (improvement && belowMaxRuntime()) {
        c++;

        std::vector<gainPair> gainPairs = {};

        auto dValues = computeDValues(partition);


        //select n gain pairs
        while (gainPairs.size() < maxGP && belowMaxRuntime()) {

            auto gainPair = findGainPair(dValues, partition);
            node a = partition.A[gainPair.a];
            node b = partition.B[gainPair.b];

            gainPairs.push_back(gainPair);


            for (uint i = 0; i < partition.A.size(); i++) {
                if (i != gainPair.a && partition.active[partition.A[i]])
                    dValues[partition.A[i]] += 2 * getCost(partition.A[i], a) - 2 * getCost(partition.A[i], b);
            }

            for (uint i = 0; i < partition.B.size(); i++) {
                if (i != gainPair.a && partition.active[partition.B[i]])
                    dValues[partition.B[i]] += 2 * getCost(partition.B[i], b) - 2 * getCost(partition.B[i], a);
            }


            //effectively remove the gain pair
            partition.active[a] = false;
            partition.active[b] = false;

        }

        //maximise gain
        std::tuple<uint64_t, edgeweight> k_G = maxGainSumIndex(gainPairs);
        phase1GPs_.push_back(gainPairs.size());
        phase1ks_.push_back(std::get<0>(k_G));
        phase1improvements_.push_back(std::get<1>(k_G));

        //reset active
        partition.active = std::vector<bool>(2 * (uId_ + 1), true);


        if (std::get<1>(k_G) <= 0) {
            improvement = false;
        } else {
            // execute swaps
            for (uint64_t i = 0; i <= std::get<0>(k_G); i++) {
                auto gainPair = gainPairs[i];
                std::swap(partition.A[gainPair.a], partition.B[gainPair.b]);
            }
        }
    }

    phase1Iters_ = c;
}


void KLHeuristic::repartitionOptimization() {
    /*
     * We need to take a phase 1 optimized partition. Then create two subgraphs, one for each partition.
     * On each subgraph use the Phase 1 optimization again, then we have a partition of each original partition.
     * Use this 4 partitions to create a new initial partition and optimize it. Return the result.*/

    if (!phase1Opt_)    //requires one McSolution to be already computed
        return;

    InducedSubgraph part0Subgraph = getInducedSubgraph(*originalGraph_, solution_.getPartition0());
    InducedSubgraph part1Subgraph = getInducedSubgraph(*originalGraph_, solution_.getPartition1());

    assert((part0Subgraph.subgraph.numberOfNodes() + part1Subgraph.subgraph.numberOfNodes()) == n_);

    long remainder = maxRuntime_ - getTimeElapsed();

    //run heuristic on partition subgraphs
    KLHeuristic part0Heuristic(&part0Subgraph.subgraph, remainder / 4);
    KLHeuristic part1Heuristic(&part1Subgraph.subgraph, remainder / 4);

    part0Heuristic.phase1Optimization();
    part1Heuristic.phase1Optimization();

    phase2Iters0_ = part0Heuristic.phase1Iters_;
    phase2Iters1_ = part1Heuristic.phase1Iters_;

    McSolution part0Sol = part0Heuristic.getPrimalSolution();
    McSolution part1Sol = part1Heuristic.getPrimalSolution();

    assert(part0Sol.getPartition0().size() + part0Sol.getPartition1().size() == part0Subgraph.subgraph.numberOfNodes());
    assert(part1Sol.getPartition0().size() + part1Sol.getPartition1().size() == part1Subgraph.subgraph.numberOfNodes());


    // build new partition from the 2 subgraph solutions
    KLPartition newPartition;
    auto part00 = part0Sol.getPartition0();
    auto part01 = part0Sol.getPartition1();
    auto part10 = part1Sol.getPartition0();
    auto part11 = part1Sol.getPartition1();

    newPartition.A = std::vector<node>(part00.size() + part11.size());
    newPartition.B = std::vector<node>(part01.size() + part10.size());
    newPartition.active = std::vector<bool>(n_, true);

    assert(part1Sol.isValid());
    assert((part00.size() + part01.size()) == part0Subgraph.subgraph.numberOfNodes());
    assert((part10.size() + part11.size()) == part1Subgraph.subgraph.numberOfNodes());
    assert((part00.size() + part11.size() + part01.size() + part10.size()) == n_);


    uint Ac = 0;
    uint Bc = 0;

    for (auto i: part00) {
        if (part0Subgraph.sub2orig[i] < uId_)
            newPartition.A[Ac++] = part0Subgraph.sub2orig[i];
    }
    for (auto i: part11) {
        if (part1Subgraph.sub2orig[i] < uId_)
            newPartition.A[Ac++] = part1Subgraph.sub2orig[i];
    }

    for (auto i: part01) {
        if (part0Subgraph.sub2orig[i] < uId_)
            newPartition.B[Bc++] = part0Subgraph.sub2orig[i];
    }
    for (auto i: part10) {
        if (part1Subgraph.sub2orig[i] < uId_)
            newPartition.B[Bc++] = part1Subgraph.sub2orig[i];
    }

    assert(Ac == (part00.size() + part11.size()));
    assert(Bc == (part10.size() + part01.size()));
    assert(Ac + Bc == n_);

    //optimize new partition
    phase1OptimizationSubRoutine(newPartition, std::min(Ac, Bc));

    assert(newPartition.A.size() + newPartition.B.size() == n_);

    for (auto i: newPartition.A) {
        if (i < uId_)
            phase2solution_.toPartition0(i);
    }

    for (auto i: newPartition.B) {
        if (i < uId_)
            phase2solution_.toPartition1(i);
    }
    assert(phase2solution_.isValid());

    phase2Opt_ = true;
}

edgeweight KLHeuristic::getCost(node i, node j) const {
    if (i >= uId_ || j >= uId_) {
        return 0;
    } else
        return -originalGraph_->weight(i, j);
}

std::vector<edgeweight> KLHeuristic::computeDValues(KLPartition partition) const {
    auto dValues = std::vector<edgeweight>(2 * n_, 0);

    for (uint i = 0; i < partition.A.size(); i++) {
        node a = partition.A[i];
        for (unsigned long j: partition.A) {
            dValues[a] -= getCost(a, j);   //internal cost
        }
        for (unsigned long j: partition.B) {
            dValues[a] += getCost(a, j);   //external cost
        }
    }

    for (uint i = 0; i < partition.B.size(); i++) {
        node b = partition.B[i];
        for (unsigned long j: partition.B) {
            dValues[b] -= getCost(b, j);   //internal cost
        }
        for (unsigned long j: partition.A) {
            dValues[b] += getCost(b, j);   //external cost
        }
    }

    return dValues;
}

gainPair KLHeuristic::findGainPair(std::vector<edgeweight> dValues, KLPartition partition) const {
    edgeweight maxG = -std::numeric_limits<edgeweight>::infinity();
    uint a = 0, b = 0;

    for (uint i = 0; i < partition.A.size(); i++) {
        if (partition.active[partition.A[i]]) {
            for (uint j = 0; j < partition.B.size(); j++) {
                if (partition.active[partition.B[j]]) {
                    auto newVal =
                            dValues[partition.A[i]] + dValues[partition.B[j]] -
                            2 * getCost(partition.A[i], partition.B[j]);
                    if (newVal > maxG) {
                        maxG = newVal;
                        a = i;
                        b = j;
                    }
                }
            }
        }
    }
    return {a, b, maxG};
}

std::tuple<uint64_t, edgeweight> KLHeuristic::maxGainSumIndex(std::vector<gainPair> gainPairs) {
    edgeweight maxGainSum = -std::numeric_limits<edgeweight>::infinity();
    edgeweight gainSum = 0;
    uint64_t maxGainSumIndex = 0;

    for (uint64_t i = 0; i < gainPairs.size(); i++) {
        gainSum += gainPairs[i].gain;
        if (gainSum > maxGainSum) {
            maxGainSum = gainSum;
            maxGainSumIndex = i;
        }
    }

    return {maxGainSumIndex, maxGainSum};
}

void KLHeuristic::done() {
    elapsed_ = getTimeElapsed();
}

bool KLHeuristic::belowMaxRuntime() const {
    return getTimeElapsed() < maxRuntime_;
}

long KLHeuristic::getTimeElapsed() const {
    auto current = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(current - start_).count();;
}

long KLHeuristic::getTotalRuntime() const {
    return elapsed_;
}

void KLHeuristic::phase2Optimization() {
    phase1Optimization();

    if (belowMaxRuntime()) {
        repartitionOptimization();
    }
    done();
}

bool KLHeuristic::phase1Opt() const {
    return phase1Opt_;
}

bool KLHeuristic::phase2Opt() const {
    return phase2Opt_;
}

void KLHeuristic::printStats() {
    std::cout << "KlHeuristic Statistics" << std::endl << std::endl;


    std::cout << "Graph: " << n_ << " nodes, " << originalGraph_->numberOfEdges() << " edges" << std::endl;

    std::cout << "Final solution value: " << getPrimalSolution().getCutValue() << std::endl << std::endl;

    std::cout << "Phase 1 Optimized: " << (phase1Opt_ ? "true" : "false") << std::endl;

    std::cout << "Phase 1 Solution value: " << solution_.getCutValue() << std::endl;

    std::cout << "Phase 2 Optimized: " << (phase2Opt_ ? "true" : "false") << std::endl;
    if (phase2Opt_)
        std::cout << "Phase 2 Solution value: " << phase2solution_.getCutValue() << std::endl;
    std::cout << "Time elapsed: " << getTotalRuntime() / 1000000 << " s / " << maxRuntime_ / 1000000 << " s"
              << std::endl;

    std::cout << std::endl;

    std::cout << "Phase 1 Outer Loop Iterations: " << phase1Iters_ << std::endl;
    std::cout << "    #GP K Improvement" << std::endl;
    for (uint i = 0; i < phase1ks_.size(); i++) {
        std::cout << "    " << phase1GPs_[i] << " " << phase1ks_[i] << " " << phase1improvements_[i] << std::endl;
    }

    if (phase2Opt_) {
        std::cout << std::endl << "Phase 2 Statistics:" << std::endl;
        std::cout << phase2Iters0_ << " Phase 1 Iterations in subgraph 0" << std::endl;
        std::cout << phase2Iters1_ << " Phase 1 Iterations in subgraph 1" << std::endl;

    }
}

uint KLHeuristic::getPhase1Iters() const {
    return phase1Iters_;
}
