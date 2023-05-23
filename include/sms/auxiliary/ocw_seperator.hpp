#ifndef SMS_OCW_SEPERATOR_HPP
#define SMS_OCW_SEPERATOR_HPP

#include <optional>
#include "tlx/container/d_ary_addressable_int_heap.hpp"
#include "networkit/graph/Graph.hpp"
#include "networkit/auxiliary/VectorComparator.hpp"
#include "sms/auxiliary/odd_closed_walk.hpp"

using NetworKit::node;
using NetworKit::edgeweight;
using NetworKit::Edge;
using NetworKit::Graph;

#define EPSILON 0.0


class OcwSeparator {
private:
    // const
    edgeweight const kInfDist = std::numeric_limits<edgeweight>::max();
    node const kNoPrevious = std::numeric_limits<node>::max();

    // Graphs
    Graph const *const originalGraph_;
    uint64_t const n_;
    Graph lpWeightedGraph_;

    // Output
    std::vector<uint64_t> cycles_;

    // Config
    bool moreViaDistanceSymmetry_;

    // SSSP
    std::vector<edgeweight> distances_;
    std::vector<node> previous_;
    node currentSource_;

    // Dijkstra specific
    tlx::d_ary_addressable_int_heap<node, 2, Aux::LessInVector<double>> heap_;

    // Previous used
    std::vector<bool> onOcw_;

public:
    // metrics
    uint64_t seenNodes = 0;

    explicit OcwSeparator(Graph const *const originalGraph, bool moreViaDistanceSymmetry = false)
            : originalGraph_(originalGraph),
              n_(originalGraph->numberOfNodes()),
              lpWeightedGraph_(originalGraph->numberOfNodes() * 2 + 1, true, false, false),
              moreViaDistanceSymmetry_(moreViaDistanceSymmetry),
              currentSource_(kNoPrevious),
              heap_(Aux::LessInVector<double>{distances_}) {
        assert(originalGraph_->numberOfNodes() == originalGraph_->upperNodeIdBound());

        distances_ = std::vector<edgeweight>(2 * n_ + 1, kInfDist);
        previous_ = std::vector<node>(2 * n_ + 1, kNoPrevious);
        onOcw_ = std::vector<bool>(2 * n_ + 1, false);
    }


    void updateWeights(node u, node v, edgeweight w) {
        assert(u < originalGraph_->numberOfNodes());
        assert(v < originalGraph_->numberOfNodes());

        // Guarantee numerical stability at 0 and 1
        edgeweight fixedVal = std::min(std::max(0., w), 1.);

        node uTwin = twinId(u);
        node vTwin = twinId(v);

        lpWeightedGraph_.setWeight(u, v, fixedVal);
        lpWeightedGraph_.setWeight(uTwin, vTwin, fixedVal);
        lpWeightedGraph_.setWeight(u, vTwin, 1 - fixedVal);
        lpWeightedGraph_.setWeight(uTwin, v, 1 - fixedVal);
    }

    OddClosedWalk getViolatedWalkViaSymmetry(node u) {
        node intermediateNode = u;
        node intermediateNodeTwin = twinId(u);
        assert(distances_[intermediateNode] != kInfDist);
        assert(distances_[intermediateNodeTwin] != kInfDist);
        assert(distances_[intermediateNode] + distances_[intermediateNodeTwin] < 1);

        OddClosedWalk ocw(originalId(intermediateNode));
        auto currentNode = intermediateNode;
        onOcw_[u] = true;
        while (currentNode != currentSource_) {
            node prevNode = previous_[currentNode];
            node origPrevNode = originalId(prevNode);
            onOcw_[origPrevNode] = true;
            if (sameSide(currentNode, prevNode)) {
                ocw.addStayEdge(origPrevNode);
            } else {
                ocw.addCrossEdge(origPrevNode);
            }
            currentNode = prevNode;
        }
        ocw.inverse();

        currentNode = intermediateNodeTwin;
        while (currentNode != currentSource_) {
            node prevNode = previous_[currentNode];
            node origPrevNode = originalId(prevNode);
            onOcw_[origPrevNode] = true;
            if (sameSide(currentNode, prevNode)) {
                ocw.addStayEdge(origPrevNode);
            } else {
                ocw.addCrossEdge(origPrevNode);
            }
            currentNode = prevNode;
        }

        assert(ocw.isValid(originalGraph_));
        return ocw;
    }

    std::optional<OddClosedWalk> getMostViolatedOcw(node source) {
        currentSource_ = source;
        node sourceTwin = twinId(currentSource_);
        node currentNode = sourceTwin;
        std::fill(onOcw_.begin(), onOcw_.end(), false);

        runDijsktra();

        if (distances_[sourceTwin] == kInfDist) {
            return {};
        }

        OddClosedWalk ocw(originalId(currentNode));
        while (currentNode != currentSource_) {
            node prevNode = previous_[currentNode];
            node origPrevNode = originalId(prevNode);
            onOcw_[origPrevNode] = true;
            // MightDo (JC): Micro-optimize potential (get rid of if)
            if (sameSide(currentNode, prevNode)) {
                ocw.addStayEdge(origPrevNode);
            } else {
                ocw.addCrossEdge(origPrevNode);
            }
            currentNode = prevNode;
        }

        return {ocw};
    }

    std::vector<OddClosedWalk> simpleOddCycles(const OddClosedWalk &ocw) const {
        assert(ocw.isValid());
        std::vector<OddClosedWalk> simpleOddCycles = {};

        // traverse cycle and see if a node was already visited
        const uint64_t NONE = std::numeric_limits<uint64_t>::max();
        auto firstFoundPosition = std::vector<uint64_t>(n_, NONE);
        uint64_t lastLeftPos = NONE;
        uint64_t lastRightPos = NONE;
        for (uint64_t i = 0; i < ocw.size(); i++) {
            auto cNode = ocw.getIthNode(i);
            if (firstFoundPosition[cNode] != NONE) {
                // no full inner seen before, or safe to start new one
                if (((lastLeftPos == NONE) && (lastRightPos == NONE)) || (lastLeftPos < firstFoundPosition[cNode])) {
                    // first simple inner cycle found
                    lastLeftPos = firstFoundPosition[cNode];
                    lastRightPos = i;

                    OddClosedWalk simpleOddClosedWalk = ocw.extract(lastLeftPos, lastRightPos);
                    assert(simpleOddClosedWalk.isSimple());
                    simpleOddCycles.push_back(simpleOddClosedWalk);
                }
            } else {
                firstFoundPosition[cNode] = i;
            }
        }
        if (simpleOddCycles.empty()) {
            assert(ocw.isSimple());
            simpleOddCycles.push_back(ocw);
        }
        return simpleOddCycles;
    }

    std::vector<OddClosedWalk> extractCycles(const OddClosedWalk &ocw) const {
        assert(ocw.isValid());
        assert(ocw.isSimple());
        std::vector<OddClosedWalk> result = {};

        uint64_t cycleLength = ocw.size();

        // precompute values
        auto crossingUpTo = std::vector<uint64_t>(cycleLength + 1, 0);
        auto lhsUpTo = std::vector<double>(cycleLength + 1, 0.);

        uint64_t cCrossingUpTo = 0;
        double cLhsUpTo = 0;
        for (uint64_t i = 1; i <= cycleLength; i++) {
            auto curNode = ocw.getIthNode(i);
            auto prevNode = ocw.getIthNode(i - 1);
            cCrossingUpTo += (ocw.ithEdgeIsCross(i - 1) ? 1 : 0);
            cLhsUpTo += ((ocw.ithEdgeIsCross(i - 1) ? 1 : -1) *
                         lpWeightedGraph_.weight(curNode, prevNode));
            crossingUpTo[i] = cCrossingUpTo;
            lhsUpTo[i] = cLhsUpTo;
        }


        // walk along cycle and find chords
        // skip first two nodes (there is no backwards chord possible)
        struct chord {
            uint64_t start;
            uint64_t end;
            uint64_t length;
            bool crossing;
            bool inner;
        };

        std::vector<chord> goodChords = {};
        for (uint64_t i = 2; i < cycleLength; i++) {
            auto curNode = ocw.getIthNode(i);
            for (auto cNeighbor: originalGraph_->neighborRange(curNode)) {
                // skip edges that already are part of cycle
                if (cNeighbor == ocw.getIthNode(i - 1) ||
                    (cNeighbor == ocw.getIthNode(i + 1)))
                    continue;
                // find neighbor position in cycle, if edge forms a chord
                // MightDo (JC): This can be speed up by sorting nodes
                auto backwardsChordNeighborPos = kNoPrevious;
                for (uint64_t j = 0; j < i; j++) {
                    if (ocw.getIthNode(j) == cNeighbor) {
                        backwardsChordNeighborPos = j;
                        break;
                    }
                }
                if (backwardsChordNeighborPos != kNoPrevious) {
                    auto j = backwardsChordNeighborPos;
                    // check both possible cycles
                    uint64_t innerCrossingDif = crossingUpTo[i] - crossingUpTo[j];
                    uint64_t outerCrossingDif = crossingUpTo[cycleLength] - innerCrossingDif;
                    double innerLhs = lhsUpTo[i] - lhsUpTo[j];
                    double outerLhs = lhsUpTo[cycleLength] - innerLhs;
                    edgeweight edgeLpValue = lpWeightedGraph_.weight(ocw.getIthNode(i), ocw.getIthNode(j));

                    bool innerIsEven = innerCrossingDif % 2 == 0;
                    if (innerIsEven) {
                        if (innerLhs + edgeLpValue > (double) innerCrossingDif) {
                            goodChords.push_back({j, i, i - j + 1, true, true});
                        }
                        if (outerLhs - edgeLpValue > (double) outerCrossingDif - 1) {
                            goodChords.push_back({j, i, cycleLength - i + j + 1, false, false});
                        }
                    } else {
                        if (innerLhs - edgeLpValue > (double) innerCrossingDif - 1) {
                            goodChords.push_back({j, i, i - j + 1, false, true});
                        }
                        if (outerLhs + edgeLpValue > (double) outerCrossingDif) {
                            goodChords.push_back({j, i, cycleLength - i + j + 1, true, false});
                        }
                    }
                }
            }
        }

        // sort good chords by their corresponding cycle length
        std::sort(goodChords.begin(), goodChords.end(), [](auto c1, auto c2) {
            return c1.length < c2.length;
        });

        // loop over chords
        auto coveredNodes = std::vector<bool>(n_, false);
        // add ocw when nodes are not covered yet
        for (auto cChord: goodChords) {
            if (!coveredNodes[cChord.start] && !coveredNodes[cChord.end]) {
                auto newOcw = ocw.splitOnChord(cChord.start, cChord.end, cChord.inner, cChord.crossing);
                assert(newOcw.isValid(originalGraph_));
                assert(newOcw.isSimple());
                result.push_back(newOcw);
                for (uint64_t i = 1; i < newOcw.size(); i++) {
                    coveredNodes[newOcw.getIthNode(i)] = true;
                }
            }
        }
        if (result.empty()) {
            return {ocw};
        }
        return result;
    }


    std::vector<OddClosedWalk> getViolatedOcws(node source) {
        currentSource_ = source;

        auto mostViolated = getMostViolatedOcw(source);
        if (!mostViolated) {
            return {};
        }
        assert(mostViolated->isValid(originalGraph_));

        std::vector<OddClosedWalk> startOcws = {*mostViolated};
        if (moreViaDistanceSymmetry_) {
            for (auto u: originalGraph_->nodeRange()) {
                if (!onOcw_[u] && (distances_[u] + distances_[twinId(u)] < 0.999999)) {
                    startOcws.push_back(getViolatedWalkViaSymmetry(u));
                }
            }
        }

        std::vector<OddClosedWalk> violatedOcws;
        for (auto &currentStartOcw: startOcws) {
            const std::vector<OddClosedWalk> simpleOcws = simpleOddCycles(currentStartOcw);
            assert(!simpleOcws.empty());
            for (auto &cSimpleOcw: simpleOcws) {
                assert(cSimpleOcw.isValid(originalGraph_));
                const std::vector<OddClosedWalk> extractedOcws = extractCycles(cSimpleOcw);
                assert(!extractedOcws.empty());
                for (auto &extractedOcw: extractedOcws) {
                    assert(extractedOcw.isValid(originalGraph_));
                    violatedOcws.push_back(extractedOcw);
                }
            }
        }


        return violatedOcws;
    }


private:

    inline node twinId(node u) const {
        return 2 * n_ - u;
    }

    inline node originalId(node u) const {
        return u > n_ ? 2 * n_ - u : u;
    }

    inline bool distanceFix(node u) const {
        return (!heap_.contains(u)) && (distances_[u] != kInfDist);
    }

    inline bool sameSide(node u, node v) const {
        return (side(u) && side(v)) || (!side(u) && !side(v));
    }

    inline bool side(node u) const {
        return u < n_;
    }

// __attribute__ ((noinline))
    void runDijsktra() {
        assert(originalGraph_->hasNode(currentSource_));
        node target = twinId(currentSource_);

        // reset data
        std::fill(distances_.begin(), distances_.end(), kInfDist);
        std::fill(previous_.begin(), previous_.end(), n_); // n can never be the previous
        heap_.clear();

        // start
        distances_[currentSource_] = 0.;
        heap_.push(currentSource_);
        // dijkstra
        do {
            node current = heap_.extract_top();
            node currentTwin = twinId(current);

            // Done, target is too far or we found it
            if ((distances_[current] >= 1 + EPSILON) || (current == target))
                break;
            // No need for neighbor exploration, twin is too far
            if (distanceFix(currentTwin)) {
                if (distances_[current] + distances_[currentTwin] >= 1 + EPSILON)
                    continue;
            } else {
                if (distances_[current] >= 0.5 + EPSILON)
                    continue;
            }

            lpWeightedGraph_.forNeighborsOf(current, [&](node neighbor, edgeweight w) {
                seenNodes += 1;
                double newDist = distances_[current] + w;

                if (newDist < 1 + EPSILON) {
                    if (newDist < distances_[neighbor]) {
                        distances_[neighbor] = newDist;
                        previous_[neighbor] = current;
                        heap_.update(neighbor);
                    } else if (distances_[neighbor] == kInfDist) {
                        distances_[neighbor] = newDist;
                        previous_[neighbor] = current;
                        heap_.push(neighbor);
                    }
                }
            });
        } while (!heap_.empty());
    }

};


#endif //SMS_OCW_SEPERATOR_HPP
