#include "sms/pheur/pheur_mqlib.hpp"
#include <iostream>
#include "sms/auxiliary/mc_solution.hpp"
#include "sms/probdata/mc_rooted.hpp"

#define HEUR_NAME             "bur02"
#define HEUR_DESC             "MQlib BURER2002 for MaxCut"
#define HEUR_DISPCHAR         'M'
#define HEUR_PRIORITY         (1000000)
#define HEUR_FREQ             1
#define HEUR_FREQOFS          0
#define HEUR_MAXDEPTH         0
#define HEUR_TIMING           SCIP_HEURTIMING_AFTERLPNODE
#define HEUR_USESSUBSCIP      FALSE

#define DEFAULT_MAXSECONDS    2.0

PHeurMQLib::PHeurMQLib(SCIP *scip, const Graph *g)
        : ObjHeur(scip, HEUR_NAME, HEUR_DESC, HEUR_DISPCHAR,
                  HEUR_PRIORITY, HEUR_FREQ, HEUR_FREQOFS, HEUR_MAXDEPTH,
                  HEUR_TIMING, FALSE),
          mcGraph_(g),
          mqlibEdgelist_(0),
          nCalls_(0),
          nSuccess_(0),
          heurRuntime_(DEFAULT_MAXSECONDS) {
    SCIP_CALL_EXC_NO_THROW(SCIPaddRealParam(scip, "heuristics/" HEUR_NAME "/maxseconds",
            "maximal number of seconds to run the heuristic",
            &heurRuntime_, FALSE, DEFAULT_MAXSECONDS, 0.0, SCIP_REAL_MAX, nullptr, nullptr))

}

/** destructor of primal heuristic to free user data (called when SCIP is exiting) */
SCIP_RETCODE PHeurMQLib::scip_free(SCIP */*scip*/, SCIP_HEUR */*heur*/) {
    return SCIP_OKAY;
}


/** initialization method of primal heuristic (called after problem was transformed) */
SCIP_RETCODE PHeurMQLib::scip_init(SCIP */*scip*/, SCIP_HEUR */*heur*/) {
    initMqlibEdgelist();
    return SCIP_OKAY;
}

/** deinitialization method of primal heuristic (called before transformed problem is freed) */
SCIP_RETCODE PHeurMQLib::scip_exit(SCIP */*scip*/, SCIP_HEUR */*heur*/) {
    return SCIP_OKAY;
}


/** solving process initialization method of primal heuristic (called when branch and bound process is about to begin)
 *
 *  This method is called when the presolving was finished and the branch and bound process is about to begin.
 *  The primal heuristic may use this call to initialize its branch and bound specific data.
 *
 */
SCIP_RETCODE PHeurMQLib::scip_initsol(SCIP * /*scip*/, SCIP_HEUR */*heur*/) {
    nCalls_ = 0;
    return SCIP_OKAY;
}


/** solving process deinitialization method of primal heuristic (called before branch and bound process data is freed)
 *
 *  This method is called before the branch and bound process is freed.
 *  The primal heuristic should use this call to clean up its branch and bound data.
 */
SCIP_RETCODE PHeurMQLib::scip_exitsol(SCIP */*scip*/, SCIP_HEUR */*heur*/) {
    return SCIP_OKAY;

}

SCIP_RETCODE
PHeurMQLib::scip_exec(SCIP *scip, SCIP_HEUR *heur, SCIP_HEURTIMING /*heurtiming*/, unsigned int /*nodeinfeasible*/,
                      SCIP_RESULT *result) {
    assert(scip != nullptr);
    assert(result != nullptr);
    assert(heur != nullptr);

    *result = SCIP_DIDNOTFIND;
    nCalls_ += 1;


    int numberOfNodes = int(mcGraph_->numberOfNodes());

    mqlib::MaxCutInstance mi(mqlibEdgelist_, numberOfNodes);
    mqlib::Burer2002 mqHeur(mi, heurRuntime_, false, nullptr);

    const mqlib::MaxCutSimpleSolution &mqlibSol = mqHeur.get_best_solution();
    const std::vector<int> &mqlibAssignment = mqlibSol.get_assignments();

    McSolution smsMcSol = McSolution(mcGraph_);

    auto *probData = SCIPgetObjProbData(scip);
    auto probDataRootedMc = dynamic_cast<ProbDataRootedMc *>(probData);

    bool flip = (mqlibAssignment[probDataRootedMc->getRoot()] + 1) / 2; // Values are -1 and 1 in MQLib
    for (size_t i = 0; i < mqlibAssignment.size(); i++) {
        bool mqLibSide = (mqlibAssignment[i] + 1) / 2; // Values are -1 and 1 in MQLib
        if (mqLibSide ^ flip)
            smsMcSol.toPartition1(i);
        else
            smsMcSol.toPartition0(i);
    }

    SCIP_SOL *newSol;
    SCIP_CALL(SCIPcreateSol(scip, &newSol, heur));

    for (auto u: smsMcSol.getPartition0()) {
        if (u != probDataRootedMc->getRoot()) {
            SCIP_CALL(SCIPsetSolVal(scip, newSol, probDataRootedMc->nodeToVar(u), 0));
        }
    }
    for (auto u: smsMcSol.getPartition1()) {
        if (u != probDataRootedMc->getRoot()) {
            SCIP_CALL(SCIPsetSolVal(scip, newSol, probDataRootedMc->nodeToVar(u), 1));
        }
    }

    for (auto e: smsMcSol.getCutEdges()) {
        SCIP_Real edgeVarValue = 1; // all other edges are init to 0 through solution constructor
        SCIP_CALL(SCIPsetSolVal(scip, newSol, probDataRootedMc->edgeToVar(e.u, e.v), edgeVarValue));
    }

    assert(!SCIPsolIsPartial(newSol));
    assert(!SCIPsolIsOriginal(newSol));


    SCIP_Bool success;
    // try heuristic solution AND ADD IT to SCIP solution list (SCIP function name is misleading)
    SCIP_CALL(SCIPtrySolFree(scip, &newSol, TRUE, FALSE, FALSE, FALSE, TRUE, &success));

    if (success) {
        nSuccess_ += 1;
        *result = SCIP_FOUNDSOL;
    }

    return SCIP_OKAY;
}


/** clone method which will be used to copy a objective plugin */
scip::ObjCloneable *PHeurMQLib::clone(SCIP *scip) const {
    return new PHeurMQLib(scip, mcGraph_);
}


