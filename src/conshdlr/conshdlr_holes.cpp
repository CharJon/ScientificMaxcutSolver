#include "sms/conshdlr/conshdlr_holes.hpp"

#include <cassert>
#include <iostream>
#include "networkit/graph/Graph.hpp"
#include "scip/cons_linear.h"
#include "scip/lp.h"
#include "objscip/objscip.h"
#include "sms/probdata/mc_rooted.hpp"
#include "sms/auxiliary/small_ccs.hpp"

/* constraint handler properties */
#define CONSHDLR_NAME          "holes"
#define CONSHDLR_DESC          "hole constraints"
#define CONSHDLR_SEPAPRIORITY   (700000) /**< priority of the constraint handler for separation */
#define CONSHDLR_ENFOPRIORITY   (-600000) /**< priority of the constraint handler for constraint enforcing */
#define CONSHDLR_CHECKPRIORITY  (-600000) /**< priority of the constraint handler for checking feasibility */
#define CONSHDLR_SEPAFREQ           (1) /**< frequency for separating cuts; zero means to separate only in the root node */
#define CONSHDLR_PROPFREQ           (1) /**< frequency for propagating domains; zero means only preprocessing propagation */
#define CONSHDLR_EAGERFREQ          (100) /**< frequency for using all instead of only the useful constraints in separation,
                                         *   propagation and enforcement, -1 for no eager evaluations, 0 for first only */
#define CONSHDLR_MAXPREROUNDS        (-1) /**< maximal number of presolving rounds the constraint handler participates in (-1: no limit) */
#define CONSHDLR_DELAYSEPA        FALSE /**< should separation method be delayed, if other separators found cuts? */
#define CONSHDLR_DELAYPROP        FALSE /**< should propagation method be delayed, if other propagators found reductions? */
#define CONSHDLR_NEEDSCONS         FALSE /**< should the constraint handler be skipped, if no constraints are available? */
#define CONSHDLR_PROP_TIMING             SCIP_PROPTIMING_BEFORELP
#define CONSHDLR_PRESOLTIMING            SCIP_PRESOLTIMING_ALWAYS

#define CONSHDLR_SEPA_CUTPOOL FALSE /**< should separated constraints be added to the cutpool? */
#define CONSHDLR_SEPA_MINEFFICACY (0.0) /**< minimal efficacy of a cut to be added to the sepastore. Has no effect if cuts are added to the cutpool! */


using NetworKit::Graph;


ConshdlrHoles::ConshdlrHoles(Scip *scip, const Graph *sepGraph) : scip::ObjConshdlr(scip, CONSHDLR_NAME,
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
                                                                  sepGraph_(sepGraph),
                                                                  holeSeparator_(sepGraph),
                                                                  sepaCutpool_(CONSHDLR_SEPA_CUTPOOL),
                                                                  minEfficacy_(CONSHDLR_SEPA_MINEFFICACY) {
    SCIP_CALL_EXC_NO_THROW(SCIPaddBoolParam(scip,
                                            "conshdlr/" CONSHDLR_NAME "/sepa_cutpool",
            "separate cuts into cutpool",
            &sepaCutpool_,
            FALSE, CONSHDLR_SEPA_CUTPOOL, nullptr, nullptr))
    SCIP_CALL_EXC_NO_THROW(SCIPaddRealParam(scip,
                                            "conshdlr/" CONSHDLR_NAME "/sepa_minefficacy",
            "minimal efficacy of a cut to be added to the sepastore. Has no effect if cuts are added to the cutpool",
            &minEfficacy_,
            FALSE, CONSHDLR_SEPA_MINEFFICACY, 0.0, 1.0, nullptr, nullptr))
}


SCIP_RETCODE
ConshdlrHoles::scip_check(SCIP * /*scip*/, SCIP_CONSHDLR * /*conshdlr*/, SCIP_CONS ** /*conss*/, int /*nconss*/,
                          SCIP_SOL * /*sol*/,
                          SCIP_Bool /*checkintegrality*/, SCIP_Bool /*checklprows*/,
                          SCIP_Bool /*printreason*/, SCIP_Bool /*completely*/,
                          SCIP_RESULT *result) {
    assert(result != nullptr);
    *result = SCIP_FEASIBLE;
    return SCIP_OKAY;
}


SCIP_RETCODE
ConshdlrHoles::scip_sepasol(SCIP * /*scip*/, SCIP_CONSHDLR * /*conshdlr*/, SCIP_CONS ** /*conss*/, int /*nconss*/,
                            int /*nusefulconss*/, SCIP_SOL */*sol*/, SCIP_RESULT */*result*/) {
    throw std::runtime_error("Not implemented: scip_sepasol.");
}

SCIP_RETCODE ConshdlrHoles::scip_sepalp(SCIP *scip, SCIP_CONSHDLR *conshdlr, SCIP_CONS **conss, int nconss,
                                        int nusefulconss, SCIP_RESULT *result) {
    SCIP_CALL(sepaHolesLp(scip, conshdlr, conss, nconss, nusefulconss, result));
    return SCIP_OKAY;
}

SCIP_RETCODE ConshdlrHoles::sepaHolesLp(
        SCIP *scip,               /**< SCIP data structure */
        SCIP_CONSHDLR *conshdlr,  /**< the constraint handler itself */
        SCIP_CONS **/*conss*/,        /**< array of constraints to process */
        int /*nconss*/,               /**< number of constraints to process */
        int /*nusefulconss*/,         /**< number of useful (non-obsolete) constraints to process */
        SCIP_RESULT *result) {
    assert(scip != nullptr);
    *result = SCIP_DIDNOTFIND;

    auto *probData = SCIPgetObjProbData(scip);
    auto probDataRootedMc = dynamic_cast<ProbDataEdgesMc *>(probData);
    assert(probDataRootedMc != nullptr);

    // Update lp values
    for (auto e: sepGraph_->edgeRange()) {
        auto var = probDataRootedMc->edgeToVar(e.u, e.v);
        auto val = SCIPgetSolVal(scip, nullptr, var);
        auto fixedVal = std::min(std::max(0., val), 1.);
        holeSeparator_.updateWeights(e.u, e.v, fixedVal);
    }

    auto violatedOcws = holeSeparator_.getViolatedFourHoles();
    unsigned int separatedCons = violatedOcws.size();
    unsigned int cutsAdded = 0;
    for (auto &violatedOcw: violatedOcws) {
        if (*result == SCIP_CUTOFF) break;
        SCIP_Real lhs = -SCIPinfinity(scip);
        ulong rhs = violatedOcw.numCrosses() - 1;

        SCIP_ROW *row;
        SCIP_CALL(SCIPcreateEmptyRowConshdlr(scip, &row, conshdlr, "oc", lhs, rhs, FALSE, FALSE,
                                             TRUE));

        SCIP_CALL(SCIPcacheRowExtensions(scip, row));
        for (unsigned int i = 0; i < violatedOcw.size(); i++) {
            auto u = violatedOcw.getIthNode(i);
            auto v = violatedOcw.getIthNode(i + 1);
            auto coefficient = violatedOcw.getIthType(i);
            SCIP_CALL(SCIPaddVarToRow(scip, row, probDataRootedMc->edgeToVar(u, v), coefficient));
        }
        SCIP_CALL(SCIPflushRowExtensions(scip, row));

        // add row
        if (SCIPisCutEfficacious(scip, nullptr, row)) {
            if (sepaCutpool_) {
                SCIP_CALL(SCIPaddPoolCut(scip, row));
                cutsAdded += 1;
                *result = SCIP_SEPARATED;
            } else {
                // get settings of scip
                auto efficacy = SCIPgetCutEfficacy(scip, nullptr, row);
                if (SCIPisGE(scip, efficacy, minEfficacy_)) {
                    SCIP_Bool infeasible;
                    SCIP_CALL(SCIPaddRow(scip, row, false, &infeasible));
                    cutsAdded += 1;
                    *result = SCIP_SEPARATED;

                    if (infeasible) {
                        *result = SCIP_CUTOFF;
                    }
                }
            }
        }
        SCIP_CALL(SCIPreleaseRow(scip, &row));
    }

    // number of cuts
    int nCutsSepaStore = SCIPgetNCuts(scip);
    auto *cutpool = SCIPgetGlobalCutpool(scip);
    int nCutsCutPool = SCIPcutpoolGetNCuts(cutpool);
    std::cout << "Separated " << separatedCons << " 4-hole cuts and added " << cutsAdded << " Result is " << *result
              << " Sepastore has " << nCutsSepaStore << " cuts and cutpool has " << nCutsCutPool << " cuts."
              << std::endl;

    return SCIP_OKAY;
}


/** constraint enforcing method of constraint handler for LP solutions */
SCIP_RETCODE ConshdlrHoles::scip_enfolp(SCIP *scip, SCIP_CONSHDLR *conshdlr, SCIP_CONS **conss, int nconss,
                                        int nusefulconss,
                                        unsigned int /*solinfeasible*/, SCIP_RESULT *result) {
    assert(result != nullptr);

    for (int i = 0; i < nconss; ++i) {
        assert(conss != nullptr);
        assert(conss[i] != nullptr);
    }

    *result = SCIP_INFEASIBLE; // Infeasible does not really mean infeasible here. See SCIP docu
    SCIP_RESULT resultSepa;
    SCIP_CALL(sepaHolesLp(scip, conshdlr, conss, nconss, nusefulconss, &resultSepa));
    if (resultSepa == SCIP_SEPARATED || resultSepa == SCIP_NEWROUND) *result = SCIP_SEPARATED;
    else {
        *result = SCIP_FEASIBLE;
    }

    return SCIP_OKAY;
}

/** constraint enforcing method of constraint handler for pseudo solutions */
SCIP_RETCODE
ConshdlrHoles::scip_enfops(SCIP * /*scip*/, SCIP_CONSHDLR * /*conshdlr*/,
                           SCIP_CONS ** /*conss*/, int /*nconss*/, int /*nusefulconss*/,
                           SCIP_Bool /*solinfeasible*/, SCIP_Bool /*objinfeasible*/,
                           SCIP_RESULT *result) {
    *result = SCIP_FEASIBLE;

    throw std::runtime_error("Not implemented: scip_enfops");
}


SCIP_RETCODE
ConshdlrHoles::scip_lock(SCIP * /*scip*/, SCIP_CONSHDLR * /*conshdlr*/, SCIP_CONS * /*cons*/,
                         SCIP_LOCKTYPE /*locktype*/,
                         int /*nlockspos*/, int /*nlocksneg*/) {
    return SCIP_OKAY;
}

SCIP_RETCODE ConshdlrHoles::scip_trans(SCIP *scip, SCIP_CONSHDLR *conshdlr, SCIP_CONS *sourcecons,
                                       SCIP_CONS **targetcons) {

    //SCIP_CONSDATA *sourcedata;
    SCIP_CONSDATA *targetdata = nullptr;

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

scip::ObjProbCloneable *ConshdlrHoles::clone(SCIP *scip, SCIP_Bool *valid) const {
    assert(valid != nullptr);
    *valid = true;
    return new ConshdlrHoles(scip, sepGraph_);
}

SCIP_RETCODE
ConshdlrHoles::scip_initsol(SCIP */*scip*/, SCIP_CONSHDLR */*conshdlr*/, SCIP_CONS **/*conss*/, int /*nconss*/) {

    auto scc = SmallChordlessCycles(*sepGraph_);
    scc.run();
    for (auto t: scc.fourHoles) {
        holeSeparator_.addFourHole(t[0], t[1], t[2], t[3]);
    }

    return SCIP_OKAY;
}
