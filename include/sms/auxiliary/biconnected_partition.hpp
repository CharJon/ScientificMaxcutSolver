#ifndef SMS_BICONNECTED_PARTITION_HPP
#define SMS_BICONNECTED_PARTITION_HPP

#include <vector>
#include "networkit/graph/Graph.hpp"
#include "networkit/components/BiconnectedComponents.hpp"
#include <map>
#include <unordered_map>

using NetworKit::Graph;

class BiconnectedPartition
{
public:
      Graph const &graph;
      std::vector<std::vector<NetworKit::node>> components;
      std::vector<NetworKit::Edge> bridges;
      std::unordered_map<NetworKit::node , std::vector<NetworKit::node>> tree_map;    //node in tree to component
      //std::unordered_map<NetworKit::Edge, NetworKit::Edge> tree_edge_map;    // edge to edge in graph
      std::unordered_map<NetworKit::node, NetworKit::node> inverse_tree_map; // node in graph to node in tree
      Graph tree;

    BiconnectedPartition(const Graph &graph) : graph(graph)
    {
        tree = Graph(0);
    }

    void compute_components();

    // return if node in tree represents a separating vertex
    bool isSeparatingVertex(NetworKit::node);

};


#endif //SMS_BICONNECTED_PARTITION_HPP
