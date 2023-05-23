#ifndef SMS_MST_HEURISTIC_HPP
#define SMS_MST_HEURISTIC_HPP

#include <vector>

#include "networkit/graph/Graph.hpp"
#include "networkit/graph/KruskalMSF.hpp"
#include "networkit/graph/RandomMaximumSpanningForest.hpp"
#include "networkit/distance/BFS.hpp"

#include "sms/auxiliary/mc_solution.hpp"
#include "sms/auxiliary/odd_closed_walk.hpp"

using NetworKit::node;
using NetworKit::edgeweight;
using NetworKit::Edge;
using NetworKit::Graph;

class MSTHeuristic {
private:
    Graph const *const originalGraph_;
    Graph lpWeightedGraph_;
    Graph lpShiftedGraph_;
    Graph tree_;

    std::queue<node> queue_;
    std::vector<node> pred_;
    NetworKit::RandomMaximumSpanningForest mstAlgo_;

public:
    explicit MSTHeuristic(Graph const *const originalGraph)
            : originalGraph_(originalGraph),
              lpWeightedGraph_(*originalGraph, true, false),
              lpShiftedGraph_(*originalGraph, true, false),
              tree_(originalGraph->numberOfNodes()),
              pred_(tree_.numberOfNodes(), -1),
              mstAlgo_(lpShiftedGraph_) {

    }


    void updateWeights(node u, node v, edgeweight w);

    void computeSpanningTree();

    std::vector<OddClosedWalk> getViolatedOCs();

    McSolution getPrimalSolution(node s);

    std::vector<node> getSTPath(node u, node v);
};

#endif //SMS_MST_HEURISTIC_HPP
