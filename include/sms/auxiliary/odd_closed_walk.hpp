#ifndef SMS_ODD_CLOSED_WALK_HPP
#define SMS_ODD_CLOSED_WALK_HPP

#include "optional"

#include "networkit/graph/Graph.hpp"

#include <vector>

using NetworKit::node;
using NetworKit::edgeweight;
using NetworKit::Edge;
using NetworKit::Graph;

/***
 * Saving data for an odd closed walk
 * Start and end node are the same
 ***/
class OddClosedWalk {

    using edgetype = int8_t;

private:
    const int8_t kSTAY = -1;
    const int8_t kCROSS = 1;

    std::vector<NetworKit::node> nodes_;
    std::vector<edgetype> edgeTypes_;
    uint64_t numCrosses_;


public:

    // MightDo (JC): Copy and move constructor?!

    OddClosedWalk() = delete;

    explicit OddClosedWalk(node startNode, int64_t guessedSize = 3) {
        nodes_.reserve(guessedSize);
        nodes_.push_back(startNode);
        numCrosses_ = 0;
    }

    explicit OddClosedWalk(const std::vector<node> &nodes, const std::vector<bool> &crossEdges) {
        assert(nodes.size() == crossEdges.size());
        nodes_ = nodes;
        nodes_.push_back(nodes[0]);
        edgeTypes_ = std::vector<edgetype>(crossEdges.size(), kSTAY);
        numCrosses_ = 0;

        for (uint64_t i = 0; i < crossEdges.size(); ++i) {
            if (crossEdges[i]) {
                edgeTypes_[i] = kCROSS;
                ++numCrosses_;
            } else {
                edgeTypes_[i] = kSTAY;
            }
        }
    }

    explicit OddClosedWalk(std::vector<node> &&nodes) {
        nodes_ = nodes;
        edgeTypes_ = std::vector<edgetype>(nodes_.size(), kSTAY);
        numCrosses_ = 0;
    }

    void addCrossEdge(node nextNode) {
        nodes_.push_back(nextNode);
        edgeTypes_.push_back(kCROSS);
        numCrosses_ += 1;
    }

    void addStayEdge(node nextNode) {
        nodes_.push_back(nextNode);
        edgeTypes_.push_back(kSTAY);
    }

    void addEdge(node nextNode, edgetype et) {
        assert(et == kSTAY || et == kCROSS);
        nodes_.push_back(nextNode);
        edgeTypes_.push_back(et);
        numCrosses_ += (et == kCROSS ? 1 : 0);
    }


    inline uint64_t size() const {
        return nodes_.size() - 1;
    }

    inline uint64_t numCrosses() const {
        return numCrosses_;
    }

    inline node getIthNode(uint64_t i) const {
        assert(i < nodes_.size());
        return nodes_[i];
    }

    inline edgeweight getIthType(uint64_t i) const {
        assert(i < edgeTypes_.size());
        return edgeTypes_[i];
    }

    inline bool ithEdgeIsCross(uint64_t i) const {
        assert(i < edgeTypes_.size());
        return edgeTypes_[i] == kCROSS;
    }

    bool isSimple() const {
        std::unordered_set<node> seen;
        for (unsigned int i = 0; i < nodes_.size() - 1; ++i) {
            auto n = nodes_[i];
            if (seen.find(n) != seen.end()) {
                return false;
            }
            seen.insert(n);
        }
        return true;
    }

    bool isValid() const {
        bool startAndEndSame = (nodes_[0] == nodes_[size()]);
        bool correctLength = (edgeTypes_.size() == size());
        uint64_t numStay = 0;
        uint64_t numCross = 0;
        for (auto i: edgeTypes_) {
            if (i == kSTAY) {
                numStay += 1;
            } else if (i == kCROSS) {
                numCross += 1;
            } else {
                return false;
            }
        }

        return startAndEndSame && correctLength && (numCross == numCrosses_) && (numCrosses_ % 2 == 1);
    }

    bool isValid(NetworKit::Graph &g) const {
        bool isCycle = true;
        for (uint64_t i = 0; i < size(); i++) {
            isCycle &= g.hasEdge(nodes_[i], nodes_[i + 1]);
        }

        return isCycle & isValid();
    }

    bool isValid(NetworKit::Graph const *g) const {
        bool isCycle = true;
        for (uint64_t i = 0; i < size(); i++) {
            isCycle &= g->hasEdge(nodes_[i], nodes_[i + 1]);
        }

        return isCycle & isValid();
    }


    /***
     * Extract an inner cycle from start and end position inclusive
     * @param start
     * @param end
     * @return
     */
    OddClosedWalk extract(uint64_t start, uint64_t end) const {
        assert(nodes_[start] == nodes_[end]);
        OddClosedWalk ocw(nodes_[start]);
        for (uint64_t i = start + 1; i <= end; i++) {
            ocw.addEdge(nodes_[i], edgeTypes_[i - 1]);
        }
        assert(ocw.isValid());
        return ocw;
    }

    /***
     * Splits a cycle by using the given chord
     * @param start
     * @param end
     * @param inner shall j-...-i be be the result or i-...-j
     * @param cross shall the chord be of crossing type
     * @return
     */
    OddClosedWalk splitOnChord(uint64_t start, uint64_t end, bool inner, bool cross) const {
        assert((start < end - 1) && (end < nodes_.size() - 1));
        if (inner) {
            OddClosedWalk ocw(nodes_[start]);
            for (uint64_t i = start + 1; i <= end; i++) {
                uint64_t nextNode = nodes_[i];
                ocw.addEdge(nextNode, edgeTypes_[i - 1]);
            }
            ocw.addEdge(nodes_[start], cross ? kCROSS : kSTAY);
            return ocw;
        } else {
            OddClosedWalk ocw(nodes_[0]);
            for (uint64_t i = 1; i <= start; i++) {
                uint64_t nextNode = nodes_[i];
                ocw.addEdge(nextNode, edgeTypes_[i - 1]);
            }
            ocw.addEdge(nodes_[end], cross ? kCROSS : kSTAY);
            for (uint64_t i = end + 1; i < nodes_.size(); i++) {
                uint64_t nextNode = nodes_[i];
                ocw.addEdge(nextNode, edgeTypes_[i - 1]);
            }
            return ocw;
        }
    }

    void inverse() {
        std::reverse(nodes_.begin(), nodes_.end());
        std::reverse(edgeTypes_.begin(), edgeTypes_.end());
    }
};


std::optional<OddClosedWalk>
violatedOddSelectionClosedWalk(const std::vector<node> &nodes, const std::vector<double> &weights);

#endif //SMS_ODD_CLOSED_WALK_HPP
