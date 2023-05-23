#ifndef SMS_TRIANGLES_HPP
#define SMS_TRIANGLES_HPP


#include <optional>
#include "networkit/graph/Graph.hpp"
#include "sms/auxiliary/odd_closed_walk.hpp"

using NetworKit::node;
using NetworKit::edgeweight;
using NetworKit::Edge;
using NetworKit::Graph;


class TriangleSeparator {
private:
    Graph lpWeightedGraph_;

    std::vector<std::array<node, 3>> triangles_;

public:
    explicit TriangleSeparator(Graph const *const originalGraph)
            : lpWeightedGraph_(originalGraph->numberOfNodes(), true, false, false) {}


    void updateWeights(node u, node v, edgeweight w) {
        //assert(u < originalGraph_->numberOfNodes());
        //assert(v < originalGraph_->numberOfNodes());

        // Guarantee numerical stability at 0 and 1
        edgeweight fixedVal = std::min(std::max(0., w), 1.);

        lpWeightedGraph_.setWeight(u, v, fixedVal);
    }

    void addTriangle(node u, node v, node w) {
        triangles_.push_back({u, v, w});
    }


    std::vector<OddClosedWalk> getViolatedTriangles() {
        auto violatedTriangles = std::vector<OddClosedWalk>();

        for (auto t: triangles_) {
            auto [u, v, w] = t;

            std::vector<edgeweight> values = {
                    lpWeightedGraph_.weight(u, v),
                    lpWeightedGraph_.weight(v, w),
                    lpWeightedGraph_.weight(w, u),
            };

            auto violatedWalk = violatedOddSelectionClosedWalk({u, v, w}, values);
            if (violatedWalk.has_value()) {
                violatedTriangles.push_back(violatedWalk.value());
            }
        }

        return violatedTriangles;
    }

};


#endif //SMS_TRIANGLES_HPP
