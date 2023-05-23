#include "sms/branch/branchrule_degree.hpp"
#include "sms/probdata/mc_rooted.hpp"

#define BRANCHRULE_NAME "degree"
#define BRANCHRULE_DESC "branches on the node-variable having the highest (dynamic) degree"
#define BRANCHRULE_PRIORITY 30000 // highest default is 10000
#define BRANCHRULE_MAXDEPTH (-1) // -1 means no maximum
#define BRANCHRULE_MAXBOUNDDIST 1.0 // 1 means always
#define BRANCHRULE_DYNAMIC false

BranchruleDegree::BranchruleDegree(SCIP *scip, bool dynamic) : scip::ObjBranchrule(scip,
                                                                                   BRANCHRULE_NAME,
                                                                                   BRANCHRULE_DESC,
                                                                                   BRANCHRULE_PRIORITY,
                                                                                   BRANCHRULE_MAXDEPTH,
                                                                                   BRANCHRULE_MAXBOUNDDIST),
                                                               dynamic_(dynamic) {
    SCIP_CALL_EXC_NO_THROW(
            SCIPaddBoolParam(scip,
                             "branching/" BRANCHRULE_NAME "/dynamic", "should degree be calculated based on fee variables only",
            &dynamic_, FALSE, BRANCHRULE_DYNAMIC, nullptr, nullptr))
}

inline bool isBinary(SCIP *scip, SCIP_VAR *var) {
    auto lpVal = SCIPgetSolVal(scip, nullptr, var);
    bool isZero = SCIPisEQ(scip, lpVal, 0);
    bool isOne = SCIPisEQ(scip, lpVal, 1);
    return isZero || isOne;
}

/***
 * Calculates the (dynamic) degree of the corresponding node of a variable.
 * Returns 0 for variables that do not represent a node.
 * @param variable
 * @return the calculated degree
 */
uint64_t BranchruleDegree::degree(SCIP_VAR *variable) {
    auto *probData = SCIPgetObjProbData(scip_);
    auto pProbDataRootedMc = dynamic_cast<ProbDataRootedMc *>(probData);
    assert(pProbDataRootedMc != nullptr);

    auto root = pProbDataRootedMc->getRoot();
    auto u = pProbDataRootedMc->varToNode(variable);

    uint64_t degree = 0;
    for (auto neighbor: pProbDataRootedMc->getMcGraph()->neighborRange(u)) {
        if (neighbor != root) {
            SCIP_VAR *neighborVar = pProbDataRootedMc->nodeToVar(neighbor);
            if (!dynamic_ || !isBinary(scip_, neighborVar)) {
                degree++;
            }
        } else {
            degree++;
        }
    }

    return degree;
}


SCIP_RETCODE BranchruleDegree::scip_execlp(SCIP *scip, SCIP_BRANCHRULE * /*branchrule*/, unsigned int /*allowaddcons*/,
                                           SCIP_RESULT *result) {
    *result = SCIP_DIDNOTRUN;

    // get LP branching candidates
    int number_of_cands;
    SCIP_VAR **candidates;
    SCIP_Real *fractional_vals;
    SCIP_CALL(
            SCIPgetLPBranchCands(scip, &candidates, nullptr, &fractional_vals, nullptr, &number_of_cands, nullptr));

    uint64_t bestScore = 0;
    int bestCandIdx = -1;

    for (int i = 0; i < number_of_cands; i++) {
        SCIP_VAR *currentVar = candidates[i];
        uint64_t currentDegree = degree(currentVar);

        if (currentDegree > bestScore) {
            bestScore = currentDegree;
            bestCandIdx = i;
        }
    }

    SCIP_NODE *downchild;
    SCIP_NODE *upchild;
    SCIP_NODE *eqchild;


    SCIPbranchVar(scip_, candidates[bestCandIdx], &downchild, &eqchild, &upchild);
    assert(downchild != nullptr);
    assert(upchild != nullptr);

    *result = SCIP_BRANCHED;

    return SCIP_OKAY;
}
