#ifndef SMS_GRAPHS_HPP
#define SMS_GRAPHS_HPP

#include <vector>
#include "networkit/graph/Graph.hpp"
#include "networkit/components/ConnectedComponents.hpp"
#include "networkit/components/BiconnectedComponents.hpp"
#include "sms/auxiliary/statistics.hpp"


using NetworKit::count;
using NetworKit::edgeweight;
using NetworKit::node;
using NetworKit::Edge;
using NetworKit::Graph;


typedef struct {
    NetworKit::Graph subgraph;
    std::vector<NetworKit::node> orig2sub;
    std::vector<NetworKit::node> sub2orig;
} InducedSubgraph;

typedef struct {
    NetworKit::Graph compact_graph;
    std::unordered_map<node, node> compact2orig;
    std::unordered_map<node, node> orig2compact;
} CompactGraph;

typedef struct {
    double solution_value;
    std::vector<bool> solution_cut;
} SimpleMaxCutSolution;


void printGraphInformation(const Graph &g);

std::tuple<node, count> maxDegreeNode(const NetworKit::Graph &g);

/**
 * Returns an induced subgraph of the input graph (including potential edge weights/directions)
 *
 * The subgraph contains all nodes in the given node range and all edges which have both end points
 * in nodes.
 *
 * @param G The input graph.
 * @param first,last The range of nodes of the induced subgraph.
 * @param compact If the resulting graph shall have compact, continuous node ids, alternatively,
 * node ids of the input graph are kept.
 *
 * @return Induced subgraph.
 */
template<class InputIt>
std::pair<Graph, std::unordered_map<node, node>>
subgraphFromNodes(const Graph &G, InputIt first, InputIt last, bool compact = false) {
    count subgraphIdBound = 0;
    std::unordered_map<node, node> reverseMapping;

    if (compact) {
        for (InputIt it = first; it != last; ++it) {
            reverseMapping[*it] = subgraphIdBound;
            ++subgraphIdBound;
        }
    } else {
        subgraphIdBound = G.upperNodeIdBound();
    }

    Graph S(subgraphIdBound, G.isWeighted(), G.isDirected());

    if (compact) {
        for (auto nodeIt: reverseMapping) {
            node u = nodeIt.first;
            node localU = nodeIt.second;
            G.forNeighborsOf(u, [&](node v, edgeweight weight) {
                if (!G.isDirected() && u > v)
                    return;

                auto vMapping = reverseMapping.find(v);
                if (vMapping != reverseMapping.end())
                    S.addEdge(localU, vMapping->second, weight);
            });
        }
    } else {
        // First, delete all nodes
        for (node u = 0; u < G.upperNodeIdBound(); ++u) {
            S.removeNode(u);
        }

        // Restore all given nodes
        for (InputIt it = first; it != last; ++it) {
            S.restoreNode(*it);
        }

        G.forEdges([&](node u, node v, edgeweight w) {
            // only include edges if at least one endpoint is in nodes
            if (S.hasNode(u) && S.hasNode(v)) {
                S.addEdge(u, v, w);
            }
        });
    }

    return {S, reverseMapping};
}

InducedSubgraph getInducedSubgraph(const NetworKit::Graph &g, const std::vector<NetworKit::node> &nodes);

InducedSubgraph getInducedSubgraphNetworkit(const NetworKit::Graph &G, const std::vector<NetworKit::node> &nodes);

double getCutValue(const NetworKit::Graph &, std::vector<bool>);

CompactGraph compactGraph(const NetworKit::Graph &graph);


#endif //SMS_GRAPHS_HPP
