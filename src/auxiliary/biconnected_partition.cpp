#include "sms/auxiliary/biconnected_partition.hpp"

#include "networkit/graph/Graph.hpp"
#include "networkit/components/BiconnectedComponents.hpp"


void BiconnectedPartition::compute_components() {
    NetworKit::BiconnectedComponents generator(graph);

    generator.run();

    components = generator.getComponents();

    auto sizes = generator.getComponentSizes();

    for (uint i = 0; i < generator.numberOfComponents(); i++) {
        if (sizes[i] == 2) {
            bridges.emplace_back(components[i][0], components[i][1]);
        }
    }

    for (uint i = 0; i < generator.numberOfComponents(); i++) {
        tree_map[i] = components[i];
        tree.addNode();
    }

    for (auto i: graph.nodeRange()) {
        auto comps = generator.getComponentsOfNode(i);

        if (comps.size() > 1) {
            tree_map[tree.numberOfNodes()] = {i};       // associate new node with separating vertex
            inverse_tree_map[i] = tree.numberOfNodes();  // apply inverse mapping
            tree.addNode();                             // add node to tree

            for (auto u: comps) {
                tree.addEdge(u, inverse_tree_map[i]);

                /*for (auto v : components[u])    // find original edge that induces tree edge
                {
                    if (graph.hasEdge(i, v))
                        tree_edge_map[NetworKit::Edge(u, inverse_tree_map[i])] = NetworKit::Edge(i, v);
                }*/


            }
        } else {
            for (auto u: comps) {
                inverse_tree_map[i] = u;
            }
        }
    }


}

bool BiconnectedPartition::isSeparatingVertex(NetworKit::node u) {
    return tree_map[u].size() == 1;
}
