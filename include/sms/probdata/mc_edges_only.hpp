#ifndef SMS_MC_EDGE_HPP
#define SMS_MC_EDGE_HPP

#include "objscip/objscip.h"
#include "scip/scipdefplugins.h"
#include "networkit/graph/Graph.hpp"
#include "sms/auxiliary/scip_exception.hpp"
#include "sms/auxiliary/scip.hpp"
#include "sms/probdata/mc_edges.hpp"

using NetworKit::Graph;
using NetworKit::node;

/***
 * SCIP problem data for purely edge based model
 */
class ProbDataEdgesOnlyMc : public ProbDataEdgesMc {
public:

    explicit ProbDataEdgesOnlyMc(Scip *scip, const Graph *g) :
            scip_(scip), mcGraph_(g) {
        edgeIdToVar_ = std::vector<SCIP_VAR *>(g->upperEdgeIdBound());
        varToEdge_ = std::unordered_map<unsigned int, NetworKit::Edge>(g->upperEdgeIdBound());
    }

private:
    explicit ProbDataEdgesOnlyMc(Scip *scip,
                                 const Graph *g,
                                 std::vector<SCIP_VAR *> &edgeToVar,
                                 std::unordered_map<unsigned int, NetworKit::Edge> &varToEdge
    ) :
            scip_(scip), mcGraph_(g),
            edgeIdToVar_(std::move(edgeToVar)), varToEdge_(std::move(varToEdge)) {
    }

public:
    ~ProbDataEdgesOnlyMc() override {
        for (auto var: edgeIdToVar_) {
            if (var != nullptr) {
                SCIP_CALL_EXC_NO_THROW(SCIPreleaseVar(scip_, &var))
            }
        }
        edgeIdToVar_.clear();
    }

    SCIP_VAR *edgeToVar(NetworKit::Edge e) const override {
        assert(mcGraph_->hasEdge(e.u, e.v));
        return edgeIdToVar(mcGraph_->edgeId(e.u, e.v));
    }

    SCIP_VAR *edgeToVar(NetworKit::node u, NetworKit::node v) const override {
        assert(mcGraph_->hasEdge(u, v));
        return edgeIdToVar(mcGraph_->edgeId(u, v));
    }

    SCIP_VAR *edgeIdToVar(NetworKit::index edgeId) const override {
        return edgeIdToVar_[edgeId];
    }

    NetworKit::Edge varIdToEdge(uint64_t varIndex) override {
        return varToEdge_[varIndex];
    }

    NetworKit::Edge varToEdge(SCIP_VAR *var) override {
        auto varId = SCIPvarGetIndex(var);
        return varIdToEdge(varId);
    }

    const Graph *getMcGraph() const override {
        return mcGraph_;
    }

    void addVariable(NetworKit::node u, NetworKit::node v, SCIP_VAR *var) {
        assert(mcGraph_->hasEdge(u, v));
        auto edgeId = mcGraph_->edgeId(u, v);
        edgeIdToVar_[edgeId] = var;
        varToEdge_[SCIPvarGetIndex(var)] = NetworKit::Edge(u, v);
    }


    /** Copies user data if you want to copy it to a subscip */
    SCIP_RETCODE scip_copy(
            SCIP * /*scip*/,               /**< SCIP data structure */
            SCIP * /*sourcescip*/,         /**< source SCIP main data structure */
            SCIP_HASHMAP * /*varmap*/,             /**< a hashmap which stores the mapping of source variables to
                                              * corresponding target variables */
            SCIP_HASHMAP * /*consmap*/,            /**< a hashmap which stores the mapping of source constrains to
                                              * corresponding target constraints */
            ObjProbData **objprobdata,        /**< pointer to store the copied problem data object */
            SCIP_Bool /*global*/,             /**< create a global or a local copy? */
            SCIP_RESULT *result              /**< pointer to store the result of the call */
    ) override {
        (*objprobdata) = nullptr;
        (*result) = SCIP_DIDNOTRUN;
        return SCIP_OKAY;
    }


    SCIP_RETCODE scip_delorig(
            SCIP * /*scip*/                /**< SCIP data structure */
    ) override {
        return SCIP_OKAY;
    }

    SCIP_RETCODE scip_deltrans(
            SCIP * /*scip*/                /**< SCIP data structure */
    ) override {
        return SCIP_OKAY;
    }

    /** creates user data of transformed problem by transforming the original user problem data
     *  (called after problem was transformed)
     */
    SCIP_RETCODE scip_trans(
            SCIP *scip,
            ObjProbData **objprobdata,     /**< pointer to store the transformed problem data object */
            SCIP_Bool *deleteobject        /**< pointer to store whether SCIP should delete the object after solving */
    ) override {
        assert(objprobdata != nullptr);
        assert(deleteobject != nullptr);
        assert(mcGraph_ != nullptr);


        auto edgeToTransVar = std::vector<SCIP_VAR *>(edgeIdToVar_.size(), nullptr);

        std::unordered_map<unsigned int, NetworKit::Edge> transVarToEdge{};

        for (unsigned int edgeId = 0; edgeId < edgeIdToVar_.size(); ++edgeId) {
            auto var = edgeIdToVar_[edgeId];

            SCIP_VAR *transvar;
            SCIP_CALL(SCIPgetTransformedVar(scip, var, &transvar));

            edgeToTransVar[edgeId] = transvar;
            SCIP_CALL(SCIPcaptureVar(scip, transvar));
            transVarToEdge[SCIPvarGetIndex(transvar)] = varToEdge(var);
        }

        auto *transProbData = new ProbDataEdgesOnlyMc(scip_, mcGraph_, edgeToTransVar, transVarToEdge);

        assert(transProbData != nullptr);
        assert(objprobdata != nullptr);
        *objprobdata = transProbData;
        *deleteobject = TRUE;

        return SCIP_OKAY;
    }

private:
    Scip *scip_;

    const Graph *mcGraph_;

    std::vector<SCIP_VAR *> edgeIdToVar_;
    std::unordered_map<unsigned int, NetworKit::Edge> varToEdge_;

};

/**
 * This function inits a SCIP
 * @param scip Output parameter
 * @param g The graph
 */
void buildEdgesOnlyMcModel(Scip **s, const Graph *g);

#endif //SMS_MC_EDGE_HPP
