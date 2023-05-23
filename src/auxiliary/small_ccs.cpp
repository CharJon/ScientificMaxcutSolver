#include <vector>
#include "networkit/graph/Graph.hpp"
#include "sms/auxiliary/small_ccs.hpp"


void SmallChordlessCycles::run() {
    computeSmallCCs();
    run_ = true;
}

bool SmallChordlessCycles::hasRun() const {
    return run_;
}

void SmallChordlessCycles::computeSmallCCs() {
    for (auto u: graph.nodeRange()) {
        for (auto x: graph.neighborRange(u)) {
            if (x <= u)
                continue;
            for (auto y: graph.neighborRange(u)) {
                if (y <= x)
                    continue;
                assert((u < x) && (u < y) && (x < y));
                if (graph.hasEdge(x, y)) {
                    triangles.push_back({x, u, y});
                } else {
                    for (auto z: graph.neighborRange(x)) {
                        if (z <= u || graph.hasEdge(u, z))
                            continue;

                        if (graph.hasEdge(z, y)) {
                            fourHoles.push_back({x, u, y, z});
                        }
                    }
                }
            }
        }
    }

}

triangle sortTriangle(const std::vector<node> &inTriangle) {
    assert(inTriangle.size() == 3);

    NetworKit::node max = *std::max_element(inTriangle.begin(), inTriangle.end());
    NetworKit::node min = *std::min_element(inTriangle.begin(), inTriangle.end());
    NetworKit::node mid = inTriangle[0];

    for (auto n: inTriangle)
        if (n != min && n != max)
            mid = n;

    return {mid, min, max};
}

fourcycle sortFourCycle(const std::vector<node> &cycle, const Graph &graph) {
    assert(cycle.size() == 4);

    NetworKit::node min = *std::min_element(cycle.begin(), cycle.end());

    std::vector<NetworKit::node> possible = {};
    for (auto u: graph.neighborRange(min)) {
        if (std::find(cycle.begin(), cycle.end(), u) != cycle.end())
            possible.push_back(u);
    }

    NetworKit::node first;
    NetworKit::node third;

    if (possible[0] > possible[1]) {
        first = possible[1];
        third = possible[0];
    } else {
        first = possible[0];
        third = possible[1];
    }

    NetworKit::node last = cycle[0];

    for (auto n: cycle)
        if (n != min && n != first && n != third)
            last = n;

    return {first, min, third, last};
}
