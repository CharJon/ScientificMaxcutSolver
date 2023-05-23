#include "sms/instance/convert.hpp"

QUBO maxCutToQUBO(const MaxCut &maxcut) {
    const int dim = maxcut.getNumberOfNodes();
    QUBO q(dim);
    const Graph &g = maxcut.getGraph();

    for (auto e: g.edgeWeightRange()) {
        q.setValueU(e.u, e.v, e.weight);
        q.setValueU(e.v, e.u, e.weight);
    }

    for (auto u: g.nodeRange()) {
        double sum = 0;
        for (auto neighbor: g.neighborRange(u)) {
            sum -= g.weight(u, neighbor);
        }
        q.setValueU(u, u, sum);
    }

    return q;
}
