#ifndef SMS_HOLES_HPP
#define SMS_HOLES_HPP


#include <optional>
#include "networkit/graph/Graph.hpp"
#include "sms/auxiliary/odd_closed_walk.hpp"

using NetworKit::node;
using NetworKit::edgeweight;
using NetworKit::Edge;
using NetworKit::Graph;


class HoleSeparator {
private:
    Graph lpWeightedGraph_;

    std::vector<std::array<node, 4>> fourHoles_;

public:
    explicit HoleSeparator(Graph const *const originalGraph)
            : lpWeightedGraph_(originalGraph->numberOfNodes(), true, false, false) {}


    void updateWeights(node u, node v, edgeweight w) {
        //assert(u < originalGraph_->numberOfNodes());
        //assert(v < originalGraph_->numberOfNodes());

        // Try to guarantee numerical stability at 0 and 1
        edgeweight fixedVal = std::min(std::max(0., w), 1.);

        lpWeightedGraph_.setWeight(u, v, fixedVal);
    }

    void addFourHole(node a, node b, node c, node d) {
        fourHoles_.push_back({a, b, c, d});
    }


    std::vector<OddClosedWalk> getViolatedFourHoles() {
        auto violatedFourHoles = std::vector<OddClosedWalk>();

        for (auto h: fourHoles_) {
            auto [a, b, c, d] = h;

            std::vector<edgeweight> values = {
                    lpWeightedGraph_.weight(a, b),
                    lpWeightedGraph_.weight(b, c),
                    lpWeightedGraph_.weight(c, d),
                    lpWeightedGraph_.weight(a, d)
            };

            auto violatedWalk = violatedOddSelectionClosedWalk({a, b, c, d}, values);
            if (violatedWalk.has_value()) {
                violatedFourHoles.push_back(violatedWalk.value());
            }
        }

        return violatedFourHoles;
    }

};


#endif //SMS_HOLES_HPP
