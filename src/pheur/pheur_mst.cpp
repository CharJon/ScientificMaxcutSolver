#include "sms/pheur/pheur_mst.hpp"
#include <iostream>
#include "sms/auxiliary/mc_solution.hpp"
#include "sms/probdata/mc_rooted.hpp"


#define HEUR_NAME             "mst"
#define HEUR_DESC             "Minimum spanning tree based LP rounding heuristic"
#define HEUR_DISPCHAR         'T'
#define HEUR_PRIORITY         (10000)
#define HEUR_FREQ             1
#define HEUR_FREQOFS          0
#define HEUR_MAXDEPTH         (-1)
#define HEUR_TIMING           SCIP_HEURTIMING_AFTERLPNODE
#define HEUR_USESSUBSCIP      FALSE


PHeurMst::PHeurMst(SCIP *scip, const Graph *g) : ObjHeur(scip, HEUR_NAME, HEUR_DESC, HEUR_DISPCHAR,
                                                         HEUR_PRIORITY, HEUR_FREQ, HEUR_FREQOFS, HEUR_MAXDEPTH,
                                                         HEUR_TIMING, HEUR_USESSUBSCIP),
                                                 mcGraph_(g), heuristic_(mcGraph_) {
}

/** destructor of primal heuristic to free user data (called when SCIP is exiting) */
SCIP_RETCODE PHeurMst::scip_free(SCIP */*scip*/, SCIP_HEUR */*heur*/) {
    return SCIP_OKAY;
}


/** initialization method of primal heuristic (called after problem was transformed) */
SCIP_RETCODE PHeurMst::scip_init(SCIP */*scip*/, SCIP_HEUR */*heur*/) {
    return SCIP_OKAY;
}

/** deinitialization method of primal heuristic (called before transformed problem is freed) */
SCIP_RETCODE PHeurMst::scip_exit(SCIP */*scip*/, SCIP_HEUR */*heur*/) {
    return SCIP_OKAY;
}


/** solving process initialization method of primal heuristic (called when branch and bound process is about to begin)
 *
 *  This method is called when the presolving was finished and the branch and bound process is about to begin.
 *  The primal heuristic may use this call to initialize its branch and bound specific data.
 *
 */
SCIP_RETCODE PHeurMst::scip_initsol(SCIP * /*scip*/, SCIP_HEUR */*heur*/) {
    return SCIP_OKAY;
}


/** solving process deinitialization method of primal heuristic (called before branch and bound process data is freed)
 *
 *  This method is called before the branch and bound process is freed.
 *  The primal heuristic should use this call to clean up its branch and bound data.
 */
SCIP_RETCODE PHeurMst::scip_exitsol(SCIP */*scip*/, SCIP_HEUR */*heur*/) {
    return SCIP_OKAY;

}

SCIP_RETCODE
PHeurMst::scip_exec(SCIP *scip, SCIP_HEUR *heur, SCIP_HEURTIMING /*heurtiming*/, unsigned int /*nodeinfeasible*/,
                    SCIP_RESULT *result) {
    assert(scip != nullptr);
    assert(result != nullptr);
    assert(heur != nullptr);

    *result = SCIP_DIDNOTRUN;

    // update weights of heuristic
    auto *probData = SCIPgetObjProbData(scip);
    auto pProbDataRootedMc = dynamic_cast<ProbDataRootedMc *>(probData);
    assert(pProbDataRootedMc != nullptr);

    // Update lp values
    for (auto e: mcGraph_->edgeRange()) {
        auto var = pProbDataRootedMc->edgeToVar(e.u, e.v);
        auto val = SCIPgetSolVal(scip, nullptr, var);
        auto fixedVal = std::min(std::max(0., val), 1.);
        heuristic_.updateWeights(e.u, e.v, fixedVal);
    }

    // call spanning tree calculation on heuristic
    heuristic_.computeSpanningTree();

    // add solution to scip
    auto solution = heuristic_.getPrimalSolution(pProbDataRootedMc->getRoot());

    SCIP_SOL *newSol;
    SCIP_CALL(SCIPcreateSol(scip, &newSol, heur));

    SCIP_CALL(SCIPsetSolVal(scip, newSol, pProbDataRootedMc->nodeToVar(pProbDataRootedMc->getRoot()), 0));

    for (auto u: solution.getPartition0()) {
        SCIP_CALL(SCIPsetSolVal(scip, newSol, pProbDataRootedMc->nodeToVar(u), 0));
    }
    for (auto u: solution.getPartition1()) {
        SCIP_CALL(SCIPsetSolVal(scip, newSol, pProbDataRootedMc->nodeToVar(u), 1));
    }

    for (auto e: solution.getCutEdges()) {
        SCIP_Real edgeVarValue = 1; // all other edges are init to 0 through solution constructor
        SCIP_CALL(SCIPsetSolVal(scip, newSol, pProbDataRootedMc->edgeToVar(e.u, e.v), edgeVarValue));
    }

    assert(!SCIPsolIsPartial(newSol));
    assert(!SCIPsolIsOriginal(newSol));


    SCIP_Bool success;
    // try heuristic solution AND ADD IT to SCIP solution list (SCIP function name is misleading)
    SCIP_CALL(SCIPtrySolFree(scip, &newSol, TRUE, FALSE, FALSE, FALSE, TRUE, &success));
    // assert(success);

    if (success) {
        *result = SCIP_FOUNDSOL;
    }

    return SCIP_OKAY;
}


/** clone method which will be used to copy a objective plugin */
scip::ObjCloneable *PHeurMst::clone(SCIP *scip) const {
    return new PHeurMst(scip, mcGraph_);
}

