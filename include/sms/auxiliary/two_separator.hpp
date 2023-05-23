#include <vector>

#include "networkit/graph/Graph.hpp"
#include "networkit/components/ConnectedComponents.hpp"
#include "networkit/components/BiconnectedComponents.hpp"
#include "networkit/components/DynConnectedComponents.hpp"

using NetworKit::node;
using NetworKit::Edge;
using NetworKit::Graph;


#ifndef SMS_TWO_SEPERATOR_HPP
#define SMS_TWO_SEPERATOR_HPP

bool isBiconnected(Graph const &g);

class TwoSeparator {
public:
    Graph const &graph;
    std::vector<std::tuple<node, node>> separators;

    explicit TwoSeparator(const Graph &graph) : graph(graph) {
        assert(isBiconnected(graph));
        separators = {};
    }

    void run();

    bool hasRun() const;

private:
    bool run_ = false;
};


#endif //SMS_TWO_SEPERATOR_HPP
