#ifndef SMS_ISING_HPP
#define SMS_ISING_HPP

#include "networkit/graph/Graph.hpp"

#include "sms/auxiliary/graphs.hpp"

using NetworKit::node;
using NetworKit::Edge;
using NetworKit::Graph;

/*
 * Represents an Ising instance.
 * Data is stored in a compact form using node id from 0 to n-1.
 */
class Ising {

public:
    explicit Ising(const Graph &g, double scalingFactor = 1, double offset = 0);;

    int getNumberOfSpins() const {
        return n_;
    }

    int getNumberOfInteractions() const {
        return m_;
    }

    double getScalingFactor() const {
        return scalingFactor_;
    }

    double getOffset() const {
        return offset_;
    }

    double getSolutionValue(const std::vector<int> &solVector) const;

    const Graph &getGraph() const {
        return graph_;
    }

    node getOriginalNode(node newNode) const {
        return newToOriginalNode_.at(newNode);
    }

    node getNewNode(node originalNode) const {
        return originalToNewNode_.at(originalNode);
    }

private:
    std::string name_;
    int n_;
    int m_;
    double scalingFactor_;
    double offset_;
    Graph graph_;
    std::unordered_map<node, node> originalToNewNode_;
    std::unordered_map<node, node> newToOriginalNode_;
};

#endif //SMS_ISING_HPP
