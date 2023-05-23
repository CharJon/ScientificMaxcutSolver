#ifndef SMS_SMALL_CCS_HPP
#define SMS_SMALL_CCS_HPP

#include <vector>
#include "networkit/graph/Graph.hpp"

using NetworKit::Graph;
using NetworKit::node;
using triangle = std::array<node, 3>;
using fourcycle = std::array<node, 4>;

class SmallChordlessCycles {
public:
    Graph const &graph;
    std::vector<triangle> triangles;
    std::vector<fourcycle> fourHoles;

    explicit SmallChordlessCycles(const Graph &graph) : graph(graph) {
        triangles = std::vector<triangle>();
        fourHoles = std::vector<fourcycle>();
    }

    void run();

    bool hasRun() const;

private:
    bool run_ = false;

    void computeSmallCCs();

};

// sort a given triangle according to Lemma 2 of https://arxiv.org/pdf/1309.1051.pdf
triangle sortTriangle(const std::vector<node> &);

// sort a given 4 cycle according to Lemma 2 of https://arxiv.org/pdf/1309.1051.pdf
fourcycle sortFourCycle(const std::vector<node> &, const NetworKit::Graph &);


#endif //SMS_SMALL_CCS_HPP
