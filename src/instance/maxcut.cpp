#include "sms/instance/maxcut.hpp"


MaxCut::MaxCut(const Graph &g, double scalingFactor, double offset) :
        scalingFactor_(scalingFactor), offset_(offset) {
    n_ = static_cast<int>(g.numberOfNodes());
    m_ = static_cast<int>(g.numberOfEdges());
    auto res = compactGraph(g);
    graph_ = std::move(res.compact_graph);
    originalToNewNode_ = std::move(res.orig2compact);
    newToOriginalNode_ = std::move(res.compact2orig);
}

double MaxCut::getSolutionValue(const std::vector<int> &solVector) const {
    assert(std::all_of(solVector.begin(), solVector.end(), [](int i) { return i == 0 || i == 1; }));
    double res = 0;
    for (auto e: graph_.edgeWeightRange()) {
        res += (solVector[e.u] ^ solVector[e.v]) * e.weight;
    }
    return res * scalingFactor_ + offset_;
}
