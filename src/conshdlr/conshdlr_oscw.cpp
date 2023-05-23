#include "sms/conshdlr/conshdlr_oscw.hpp"

#include "objscip/objscip.h"
#include "scip/cons_linear.h"

#include "sms/auxiliary/ocw_seperator.hpp"
#include "sms/probdata/mc_rooted.hpp"

/* constraint handler properties */
#define CONSHDLR_NAME          "oscw"
#define CONSHDLR_DESC          "odd selection closed walk"
#define CONSHDLR_SEPAPRIORITY   (+600000) /**< priority of the constraint handler for separation */
#define CONSHDLR_ENFOPRIORITY   (-600000) /**< priority of the constraint handler for constraint enforcing */
#define CONSHDLR_CHECKPRIORITY  (-600000) /**< priority of the constraint handler for checking feasibility */
#define CONSHDLR_SEPAFREQ             5 /**< frequency for separating cuts; zero means to separate only in the root node */
#define CONSHDLR_PROPFREQ             1 /**< frequency for propagating domains; zero means only preprocessing propagation */
#define CONSHDLR_EAGERFREQ          100 /**< frequency for using all instead of only the useful constraints in separation,
                                         *   propagation and enforcement, -1 for no eager evaluations, 0 for first only */
#define CONSHDLR_MAXPREROUNDS        (-1) /**< maximal number of presolving rounds the constraint handler participates in (-1: no limit) */
#define CONSHDLR_DELAYSEPA        TRUE /**< should separation method be delayed, if other separators found cuts? */
#define CONSHDLR_DELAYPROP        FALSE /**< should propagation method be delayed, if other propagators found reductions? */
#define CONSHDLR_NEEDSCONS         TRUE /**< should the constraint handler be skipped, if no constraints are available? */
#define CONSHDLR_PROP_TIMING             SCIP_PROPTIMING_BEFORELP
#define CONSHDLR_PRESOLTIMING            SCIP_PRESOLTIMING_ALWAYS

#define CONSHDLR_SEPA_CUTPOOL            TRUE /**< should cuts get added to the cutpool or directly to the sepastore */

ConshdlrOscw::ConshdlrOscw(Scip *scip, const Graph *sepGraph) : scip::ObjConshdlr(scip, CONSHDLR_NAME,
                                                                                  CONSHDLR_DESC,
                                                                                  CONSHDLR_SEPAPRIORITY,
                                                                                  CONSHDLR_ENFOPRIORITY,
                                                                                  CONSHDLR_CHECKPRIORITY,
                                                                                  CONSHDLR_SEPAFREQ,
                                                                                  CONSHDLR_PROPFREQ,
                                                                                  CONSHDLR_EAGERFREQ,
                                                                                  CONSHDLR_MAXPREROUNDS,
                                                                                  CONSHDLR_DELAYSEPA,
                                                                                  CONSHDLR_DELAYPROP,
                                                                                  CONSHDLR_NEEDSCONS,
                                                                                  CONSHDLR_PROP_TIMING,
                                                                                  CONSHDLR_PRESOLTIMING),
                                                                sepGraph_(sepGraph), ocwSeparator_(sepGraph),
                                                                sepaCutpool_(CONSHDLR_SEPA_CUTPOOL) {
    SCIP_CALL_EXC_NO_THROW(SCIPaddBoolParam(scip,
                                            "constraints/" CONSHDLR_NAME "/sepacutpool",
            "separate cuts into cutpool",
            &sepaCutpool_,
            FALSE, CONSHDLR_SEPA_CUTPOOL, nullptr, nullptr));
}

/** Use this to add a constraint to a model */
SCIP_RETCODE SCIPcreateConsOddCyclesFast(
        SCIP *scip,                   /**< SCIP data structure */
        SCIP_CONS **cons,             /**< pointer to hold the created constraint */
        const char *name,             /**< name of constraint */
        const Graph */*graph*/,       /**< the underlying graph */
        SCIP_Bool initial,            /**< should the LP relaxation of constraint be in the initial LP? */
        SCIP_Bool separate,           /**< should the constraint be separated during LP processing? */
        SCIP_Bool enforce,            /**< should the constraint be enforced during node processing? */
        SCIP_Bool check,              /**< should the constraint be checked for feasibility? */
        SCIP_Bool propagate,          /**< should the constraint be propagated during node processing? */
        SCIP_Bool local,              /**< is constraint only valid locally? */
        SCIP_Bool modifiable,         /**< is constraint modifiable (subject to column generation)? */
        SCIP_Bool dynamic,            /**< is constraint dynamic? */
        SCIP_Bool removable           /**< should the constraint be removed from the LP due to aging or cleanup? */
) {
    SCIP_CONSHDLR *conshdlr;
    SCIP_CONSDATA *consdata = nullptr;

    /* find correct constraint handler */
    conshdlr = SCIPfindConshdlr(scip, CONSHDLR_NAME);
    if (conshdlr == nullptr) {
        SCIPerrorMessage("constraint handler not found\n");
        return SCIP_PLUGINNOTFOUND;
    }

    /* create constraint data */
    //SCIP_CALL( SCIPallocBlockMemory(scip, &consdata) ); /*lint !e530*/

    /* create constraint */
    SCIP_CALL(SCIPcreateCons(scip, cons, name, conshdlr, consdata, initial, separate, enforce, check, propagate,
                             local, modifiable, dynamic, removable, FALSE));

    return SCIP_OKAY;
}

/** creates and captures an OddCycle-MaxCut constraint */
SCIP_RETCODE SCIPcreateConsBasicOddCyclesFast(
        SCIP *scip,                   /**< SCIP data structure */
        SCIP_CONS **cons,             /**< pointer to hold the created constraint */
        const char *name,             /**< name of constraint */
        const Graph *graph           /**< the graph to separate on */
) {
    return SCIPcreateConsOddCyclesFast(scip, cons, name, graph,
                                       FALSE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE);
}

/**
 * Feasibility check method of constraint handler for primal solutions.
 */
SCIP_RETCODE
ConshdlrOscw::scip_check(SCIP *scip, SCIP_CONSHDLR * /*conshdlr*/, SCIP_CONS ** /*conss*/, int nconss,
                         SCIP_SOL *sol,
                         SCIP_Bool /*checkintegrality*/, SCIP_Bool /*checklprows*/,
                         SCIP_Bool /*printreason*/, SCIP_Bool /*completely*/,
                         SCIP_RESULT *result) {
    assert(result != nullptr);

    // get probdata
    auto *probdata = SCIPgetObjProbData(scip);
    assert(probdata != nullptr);
    auto *probDataEdgesMc = dynamic_cast<ProbDataEdgesMc *>(probdata);

    const uint8_t kUNASSIGNED = 2;

    for (int i = 0; i < nconss && *result != SCIP_INFEASIBLE; ++i) {
        std::vector<node> stack;
        std::vector<uint8_t> side(sepGraph_->upperNodeIdBound(), kUNASSIGNED);

        for (unsigned int j = 0; j < side.size(); j++) {
            if (side[j] == kUNASSIGNED) {
                stack.push_back(j);
                side[j] = 0;

                while (!stack.empty()) {
                    auto currentNode = stack.back();
                    stack.pop_back();
                    assert(side[currentNode] <= 1);

                    for (auto curNeighbor: sepGraph_->neighborRange(currentNode)) {
                        auto edgeVar = probDataEdgesMc->edgeToVar(currentNode, curNeighbor);
                        assert(isBinary(scip, edgeVar, sol));
                        auto edgeVal = SCIPgetSolVal(scip, sol, edgeVar);
                        int binaryEdgeVal = edgeVal < 0.5 ? 0 : 1;

                        if (side[curNeighbor] == 2) {
                            side[curNeighbor] = side[currentNode] ^ binaryEdgeVal;
                            stack.push_back(curNeighbor);
                        } else if (side[curNeighbor] != (side[currentNode] ^ binaryEdgeVal)) {
                            *result = SCIP_INFEASIBLE;
                            return SCIP_OKAY;
                        }
                    }
                }
            }
        }
    }

    *result = SCIP_FEASIBLE;
    return SCIP_OKAY;
}

/**
 * separation method of constraint handler for arbitrary primal solution
 */
SCIP_RETCODE
ConshdlrOscw::scip_sepasol(SCIP *scip, SCIP_CONSHDLR *conshdlr, SCIP_CONS **conss, int nconss,
                           int nusefulconss, SCIP_SOL *sol, SCIP_RESULT *result) {
    SCIP_CALL(sepaOddCycles(scip, conshdlr, conss, nconss, nusefulconss, sol, result));
    return SCIP_OKAY;
}

/**
 * separation method of constraint handler for LP solution
 */
SCIP_RETCODE ConshdlrOscw::scip_sepalp(SCIP *scip, SCIP_CONSHDLR *conshdlr, SCIP_CONS **conss, int nconss,
                                       int nusefulconss, SCIP_RESULT *result) {
    SCIP_CALL(sepaOddCyclesLp(scip, conshdlr, conss, nconss, nusefulconss, result));
    return SCIP_OKAY;
}

SCIP_RETCODE ConshdlrOscw::sepaOddCycles(SCIP *scip, SCIP_CONSHDLR *conshdlr, SCIP_CONS **, int, int, SCIP_SOL *sol,
                                         SCIP_RESULT *result) {
    assert(scip != nullptr);
    *result = SCIP_DIDNOTFIND;

    auto *probData = SCIPgetObjProbData(scip);
    auto probDataMc = dynamic_cast<ProbDataEdgesMc *>(probData);
    assert(probDataMc != nullptr);

    // Update lp values
    for (auto e: sepGraph_->edgeRange()) {
        auto var = probDataMc->edgeToVar(e.u, e.v);
        auto val = SCIPgetSolVal(scip, sol, var);
        auto fixedVal = std::min(std::max(0., val), 1.);
        ocwSeparator_.updateWeights(e.u, e.v, fixedVal);
    }

    unsigned int separatedCons = 0;
    unsigned int cutsAdded = 0;
    std::vector<int> cutsAddedLength = {};

    // Separate from each node
    for (node currentNode: sepGraph_->nodeRange()) {
        auto violatedOcws = ocwSeparator_.getViolatedOcws(currentNode);
        separatedCons += violatedOcws.size();
        for (auto &violatedOcw: violatedOcws) {
            assert(violatedOcw.isValid());
            assert(violatedOcw.isSimple());
            SCIP_Real lhs = -SCIPinfinity(scip);
            ulong rhs = violatedOcw.numCrosses() - 1;

            SCIP_ROW *row;
            SCIP_CALL(SCIPcreateEmptyRowConshdlr(scip, &row, conshdlr, "osc", lhs, rhs, FALSE, FALSE,
                                                 TRUE));

            SCIP_CALL(SCIPcacheRowExtensions(scip, row));
            for (unsigned int i = 0; i < violatedOcw.size(); i++) {
                auto u = violatedOcw.getIthNode(i);
                auto v = violatedOcw.getIthNode(i + 1);
                auto coefficient = violatedOcw.getIthType(i);
                SCIP_CALL(SCIPaddVarToRow(scip, row, probDataMc->edgeToVar(u, v), coefficient));
                totalConstructedCycleLength_ += 1;
            }
            SCIP_CALL(SCIPflushRowExtensions(scip, row));

            if (sepaCutpool_) {
                if (SCIPisCutEfficacious(scip, nullptr, row)) {
                    SCIP_CALL(SCIPaddPoolCut(scip, row));
                    cutsAdded += 1;
                    cutsAddedLength.push_back(violatedOcw.size());
                    *result = SCIP_SEPARATED;
                }
            } else {
                SCIP_Bool infeasible;
                if (SCIPisCutEfficacious(scip, nullptr, row)) {
                    SCIP_CALL(SCIPaddRow(scip, row, TRUE, &infeasible));
                    cutsAdded += 1;
                    cutsAddedLength.push_back(violatedOcw.size());
                    if (infeasible) {
                        *result = SCIP_CUTOFF;
                        return SCIP_OKAY;
                    } else {
                        *result = SCIP_SEPARATED;
                    }
                }
            }

            SCIP_CALL(SCIPreleaseRow(scip, &row));
        }
    }

    std::cout << "Separated " << separatedCons << " odd cycle cuts and added " << cutsAdded << " Result is "
              << *result;
    // print min and max length of added cuts
    if (!cutsAddedLength.empty()) {
        std::sort(cutsAddedLength.begin(), cutsAddedLength.end());
        std::cout << ". Min cut length: " << cutsAddedLength[0] << " Max cut length: " << cutsAddedLength.back();
    }
    std::cout << std::endl;

    return SCIP_OKAY;
}

/** separates odd cycle cuts */
SCIP_RETCODE ConshdlrOscw::sepaOddCyclesLp(
        SCIP *scip,               /**< SCIP data structure */
        SCIP_CONSHDLR *conshdlr,  /**< the constraint handler itself */
        SCIP_CONS **conss,        /**< array of constraints to process */
        int nconss,               /**< number of constraints to process */
        int nusefulconss,         /**< number of useful (non-obsolete) constraints to process */
        SCIP_RESULT *result) {
    return sepaOddCycles(scip, conshdlr, conss, nconss, nusefulconss, nullptr, result);
}


/** constraint enforcing method of constraint handler for LP solutions */
SCIP_RETCODE ConshdlrOscw::scip_enfolp(SCIP *scip, SCIP_CONSHDLR *conshdlr, SCIP_CONS **conss,
                                       int nconss, int nusefulconss, unsigned int /*solinfeasible*/,
                                       SCIP_RESULT *result) {
    assert(result != nullptr);
    assert(nconss > 0);

    *result = SCIP_INFEASIBLE; // Infeasible does not really mean infeasible here. See SCIP docu
    SCIP_CALL(sepaOddCyclesLp(scip, conshdlr, conss, nconss, nusefulconss, result));
    if (*result == SCIP_DIDNOTFIND) {
        *result = SCIP_FEASIBLE;
    }
    return SCIP_OKAY;
}

/** constraint enforcing method of constraint handler for pseudo solutions */
SCIP_RETCODE
ConshdlrOscw::scip_enfops(SCIP * /*scip*/, SCIP_CONSHDLR * /*conshdlr*/,
                          SCIP_CONS ** /*conss*/, int /*nconss*/, int /*nusefulconss*/,
                          SCIP_Bool /*solinfeasible*/, SCIP_Bool /*objinfeasible*/,
                          SCIP_RESULT *result) {
    assert(result != nullptr);
    *result = SCIP_FEASIBLE;
    throw std::runtime_error("Not implemented: scip_enfops");
    //return SCIP_OKAY;
}

/**
 * Variable rounding lock method of constraint handler
 * Rounding to 0 and to 1 can render a constraint violated!
 */
SCIP_RETCODE
ConshdlrOscw::scip_lock(SCIP *scip, SCIP_CONSHDLR * /*conshdlr*/, SCIP_CONS * /*cons*/,
                        SCIP_LOCKTYPE /*locktype*/, int nlockspos, int nlocksneg) {
    auto *probData = SCIPgetObjProbData(scip);
    auto probDataMc = dynamic_cast<ProbDataEdgesMc *>(probData);
    assert(probDataMc != nullptr);

    for (auto v: probDataMc->getMcGraph()->edgeRange()) {
        auto curVar = probDataMc->edgeToVar(v.u, v.v);
        SCIPaddVarLocksType(scip, curVar, SCIP_LOCKTYPE_MODEL, nlockspos + nlocksneg, nlockspos + nlocksneg);
    }

    return SCIP_OKAY;
}

/** transforms constraint data into data belonging to the transformed problem */
SCIP_RETCODE ConshdlrOscw::scip_trans(SCIP *scip, SCIP_CONSHDLR *conshdlr, SCIP_CONS *sourcecons,
                                      SCIP_CONS **targetcons) {
    //SCIP_CONSDATA *sourcedata;
    SCIP_CONSDATA *targetdata = nullptr;

    //sourcedata = SCIPconsGetData(sourcecons);
    //assert(sourcedata != nullptr);

    //SCIP_CALL(SCIPallocBlockMemory(scip, &targetdata));
    //targetdata->graph = sourcedata->graph;

    // create target constraint
    SCIP_CALL(SCIPcreateCons(scip, targetcons, SCIPconsGetName(sourcecons), conshdlr, targetdata,
                             SCIPconsIsInitial(sourcecons), SCIPconsIsSeparated(sourcecons),
                             SCIPconsIsEnforced(sourcecons),
                             SCIPconsIsChecked(sourcecons), SCIPconsIsPropagated(sourcecons),
                             SCIPconsIsLocal(sourcecons),
                             SCIPconsIsModifiable(sourcecons), SCIPconsIsDynamic(sourcecons),
                             SCIPconsIsRemovable(sourcecons),
                             SCIPconsIsStickingAtNode(sourcecons)));
    return SCIP_OKAY;
}

/** clone method which will be used to copy an objective plugin */
scip::ObjProbCloneable *ConshdlrOscw::clone(SCIP *scip, SCIP_Bool *valid) const {
    assert(valid != nullptr);
    *valid = true;
    return new ConshdlrOscw(scip, sepGraph_);
}

