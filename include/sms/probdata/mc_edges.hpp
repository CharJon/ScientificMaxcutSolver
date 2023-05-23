#ifndef SMS_MC_EDGES_HPP
#define SMS_MC_EDGES_HPP

#include "objscip/objscip.h"
#include "scip/scipdefplugins.h"

#include "networkit/graph/Graph.hpp"

using NetworKit::Graph;
using NetworKit::Edge;
using NetworKit::node;

/***
 * Abstract superclass for Max-Cut models with edge variables
 */
class ProbDataEdgesMc : public scip::ObjProbData {
public:

    virtual SCIP_VAR *edgeToVar(NetworKit::Edge e) const = 0;

    virtual SCIP_VAR *edgeToVar(NetworKit::node u, NetworKit::node v) const = 0;

    virtual SCIP_VAR *edgeIdToVar(NetworKit::index edgeId) const = 0;

    virtual NetworKit::Edge varIdToEdge(uint64_t varIndex) = 0;

    virtual NetworKit::Edge varToEdge(SCIP_VAR *var) = 0;

    virtual const Graph *getMcGraph() const = 0;

};

#endif //SMS_MC_EDGES_HPP
