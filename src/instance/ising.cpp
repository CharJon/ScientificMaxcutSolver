#include "sms/instance/ising.hpp"


Ising::Ising(const Graph &g, double scalingFactor, double offset) :
        scalingFactor_(scalingFactor), offset_(offset) {
    auto res = compactGraph(g);
    graph_ = std::move(res.compact_graph);
    n_ = static_cast<int>(graph_.numberOfNodes());
    m_ = static_cast<int>(graph_.numberOfEdges());
    originalToNewNode_ = std::move(res.orig2compact);
    newToOriginalNode_ = std::move(res.compact2orig);
}

double Ising::getSolutionValue(const std::vector<int> &solVector) const {
    assert(std::all_of(solVector.begin(), solVector.end(), [](int i) { return i == -1 || i == 1; }));
    double res = 0;
    for (auto e: graph_.edgeWeightRange()) {
        res += solVector[e.u] * solVector[e.v] * e.weight;
    }
    return res * scalingFactor_ + offset_;
}
