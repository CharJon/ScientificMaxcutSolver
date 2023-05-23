#include "sms/io/dimacs_reader.hpp"

DimacsReader::DimacsReader(const std::string &path, double scaling, bool isMinimize) {
    McObj obj = fileToMcObj(path);

    graph_ = NetworKit::Graph(obj.nodes + 1, true);

    if (isMinimize) {
        for (auto [a, b, w]: obj.edge_list) {
            graph_.addEdge(a, b, -w);
        }
    } else {
        for (auto [a, b, w]: obj.edge_list) {
            graph_.addEdge(a, b, w);
        }
    }

    graph_.removeNode(0); // remove 0 as files are 1 based

    scaling_ = scaling;
}

Ising DimacsReader::getInstance() {
    return Ising(graph_, scaling_);
}

NetworKit::Graph DimacsReader::getGraph() {
    return graph_;
}



