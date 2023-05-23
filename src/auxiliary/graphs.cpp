#include <vector>
#include <map>

#include "networkit/graph/Graph.hpp"
#include "networkit/clique/MaximalCliques.hpp"
#include "networkit/components/ConnectedComponents.hpp"
#include "networkit/components/BiconnectedComponents.hpp"

#include "sms/auxiliary/graphs.hpp"
#include "sms/auxiliary/statistics.hpp"
#include "sms/auxiliary/small_ccs.hpp"


using NetworKit::node;
using NetworKit::Graph;

void printGraphInformation(const Graph &g) {
    std::cout << "---------- Graph statistics -----------------" << std::endl;
    std::cout << "Graph has " << g.numberOfNodes() << " nodes and " << g.numberOfEdges() << " edges." << std::endl
              << std::endl;

    // degrees
    std::vector<ulong> degrees;
    for (auto v: g.nodeRange()) {
        degrees.push_back(g.degree(v));
    }

    auto degree_stats = getStatistics(degrees);

    std::cout << "Degree Statistics" << std::endl;
    std::cout << "Minimal Degree     : " << degree_stats.min << std::endl;
    std::cout << "Maximal Degree     : " << degree_stats.max << std::endl;
    std::cout << "Mean Degree        : " << degree_stats.mean << std::endl;
    std::cout << "Median Degree      : " << degree_stats.median << std::endl;
    std::cout << "Standard Deviation : " << degree_stats.standard_dev << std::endl;
    std::cout << "Mode Degree        : " << degree_stats.mode << std::endl;
    std::cout << "#Degree 0          : " << std::count(degrees.begin(), degrees.end(), 0) << std::endl;
    std::cout << "#Degree 1          : " << std::count(degrees.begin(), degrees.end(), 1) << std::endl;
    std::cout << "#Degree 2          : " << std::count(degrees.begin(), degrees.end(), 2) << std::endl;
    std::cout << "#Degree 3          : " << std::count(degrees.begin(), degrees.end(), 3) << std::endl;
    std::cout << std::endl;


    // weights
    bool is_int = true;
    double intpart = 0;
    uint num_zero = 0;
    std::vector<double> weights;
    double sumPositiveWeights = 0;
    for (auto e: g.edgeWeightRange()) {
        weights.push_back(e.weight);

        if (std::modf(e.weight, &intpart) != 0.0)
            is_int = false;

        if (e.weight == 0)
            num_zero++;

        if (e.weight > 0)
            sumPositiveWeights += e.weight;
    }

    auto weight_stats = getStatistics(weights);

    std::cout << "Weight Statistics" << std::endl;
    std::cout << "Minimal Weight    : " << weight_stats.min << std::endl;
    std::cout << "Maximal Weight    : " << weight_stats.max << std::endl;
    std::cout << "Mean Weight       : " << weight_stats.mean << std::endl;
    std::cout << "Median Weight     : " << weight_stats.median << std::endl;
    std::cout << "Standard Deviation: " << weight_stats.standard_dev << std::endl;
    if (is_int)
        std::cout << "Mode of Weights   : " << weight_stats.mode << std::endl;
    std::cout << "Weights are " << (is_int ? "integer" : "floating point") << " values." << std::endl;
    std::cout << "Number of zero weight edges: " << num_zero << std::endl;
    std::cout << "Sum of positive weights: " << sumPositiveWeights << std::endl;
    std::cout << std::endl;


    // connected components

    NetworKit::ConnectedComponents comps(g);
    comps.run();

    uint num_one_comp = 0;
    std::vector<ulong> component_sizes;

    for (const auto &c: comps.getComponents()) {
        ulong size = c.size();

        if (size > 1)
            component_sizes.push_back(size);
        else
            num_one_comp++;
    }

    auto comp_stats = getStatistics(component_sizes);

    std::cout << "Connected Components" << std::endl;
    std::cout << "Number of components: " << comps.getComponents().size() << std::endl;
    std::cout << "Size of largest     : " << comp_stats.max << std::endl;
    std::cout << "Size of smallest >1 : " << comp_stats.min << std::endl;
    std::cout << "Mode of Sizes       : " << comp_stats.mode << std::endl;
    std::cout << "Number of size one components : " << num_one_comp << std::endl;
    std::cout << std::endl;


    NetworKit::BiconnectedComponents bi_cons(g);
    bi_cons.run();

    uint num_two_bi_cons = 0;
    std::vector<ulong> bi_cons_sizes;

    for (const auto &c: bi_cons.getComponents()) {
        ulong size = c.size();

        if (size > 2)
            bi_cons_sizes.push_back(size);
        else if (size == 2)
            num_two_bi_cons++;
    }

    std::cout << "Bi-Connected Components" << std::endl;
    std::cout << "Number of components: " << bi_cons.getComponents().size() << std::endl;

    if (not bi_cons_sizes.empty()) {
        auto bi_cons_stats = getStatistics(bi_cons_sizes);
        std::cout << "Size of largest     : " << bi_cons_stats.max << std::endl;
        std::cout << "Size of smallest >2 : " << bi_cons_stats.min << std::endl;
        std::cout << "Mode of Sizes       : " << bi_cons_stats.mode << std::endl;
        std::cout << "Size two components : " << num_two_bi_cons << std::endl;
        std::cout << std::endl;
    }

    // short chordless cycles
    SmallChordlessCycles ccs(g);
    ccs.run();
    std::cout << "Chordless Cycles" << std::endl;
    std::cout << "Number of triangles: " << ccs.triangles.size() << std::endl;
    std::cout << "Number of 4-holes  : " << ccs.fourHoles.size() << std::endl;
    std::cout << std::endl;

    // cliques
    auto mc = NetworKit::MaximalCliques(g);
    mc.run();
    auto cliques = mc.getCliques();

    std::vector<unsigned int> clique_sizes;

    for (const auto &i: cliques) {
        if (i.size() >= 5) clique_sizes.push_back(i.size());
    }

    std::cout << "Cliques" << std::endl;
    std::cout << "Found " << clique_sizes.size() << " cliques of size >= 5 in the graph." << std::endl;

    if (!clique_sizes.empty()) {
        std::cout << "Biggest clique found of size >= 5 is "
                  << *std::max_element(clique_sizes.begin(), clique_sizes.end())
                  << "."
                  << std::endl;
    }

    std::cout << std::endl << "-------------------------------" << std::endl << std::endl;
}

std::tuple<node, count> maxDegreeNode(const NetworKit::Graph &g) {
    NetworKit::node maxDegreeNode = 0;
    NetworKit::count maxDegree = g.degree(maxDegreeNode);
    for (auto u: g.nodeRange()) {
        NetworKit::count currentDegree = g.degree(u);
        if (currentDegree > maxDegree) {
            maxDegree = currentDegree;
            maxDegreeNode = u;
        }
    }
    return {maxDegreeNode, maxDegree};
}

InducedSubgraph getInducedSubgraph(const NetworKit::Graph &g, const std::vector<NetworKit::node> &nodes) {
    NetworKit::Graph subgraph(nodes.size(), true);
    std::vector<NetworKit::node> orig2sub(g.numberOfNodes(), NetworKit::none);

    NetworKit::node id = 0;

    for (auto v: nodes)    // set mapping from original to subgraph
    {
        orig2sub[v] = id;
        id++;
    }

    for (auto v: nodes) {
        for (auto u: g.neighborRange(v)) {
            if (orig2sub[u] != NetworKit::none) {
                subgraph.addEdge(orig2sub[v], orig2sub[u], g.weight(u, v));
            }
        }
    }

    subgraph.removeMultiEdges();

    return {subgraph, orig2sub, nodes};
}

InducedSubgraph getInducedSubgraphNetworkit(const NetworKit::Graph &g, const std::vector<NetworKit::node> &nodes) {
    auto graphAndMapping = subgraphFromNodes(g, nodes.begin(), nodes.end(), true);
    auto inducedGraph = graphAndMapping.first;
    auto reverseMapping = graphAndMapping.second;

    std::vector<NetworKit::node> orig2sub(g.numberOfNodes(), NetworKit::none);
    std::vector<NetworKit::node> sub2orig(nodes.size(), NetworKit::none);

    for (const auto keyValue: reverseMapping)    // set mapping from original to subgraph
    {
        auto key = keyValue.first;
        auto value = keyValue.second;
        sub2orig[key] = value;
        orig2sub[value] = key;
    }

    return {inducedGraph, orig2sub, sub2orig};
}

double getCutValue(const NetworKit::Graph &g, std::vector<bool> cut) {
    double cut_value = 0;

    for (auto e: g.edgeWeightRange()) {
        cut_value += e.weight * (cut[e.u] ^ cut[e.v]);
    }

    return cut_value;
}

CompactGraph compactGraph(const NetworKit::Graph &graph) {
    std::unordered_map<NetworKit::node, NetworKit::node> compact_to_orig;
    std::unordered_map<NetworKit::node, NetworKit::node> orig_to_compact;

    NetworKit::Graph compact_graph = NetworKit::Graph(graph.numberOfNodes(), true);

    NetworKit::node c = 0;
    for (auto v: graph.nodeRange()) {
        compact_to_orig[c] = v;
        orig_to_compact[v] = c;
        c++;
    }

    for (auto e: graph.edgeWeightRange()) {
        compact_graph.addEdge(orig_to_compact[e.u], orig_to_compact[e.v], e.weight);
    }

    return {compact_graph, compact_to_orig, orig_to_compact};
}




