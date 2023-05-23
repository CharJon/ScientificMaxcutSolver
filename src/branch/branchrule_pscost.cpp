#include "sms/branch/branchrule_pscost.hpp"

#include "sms/auxiliary/scip_exception.hpp"
#include "sms/probdata/mc_rooted.hpp"

#define BRANCHRULE_NAME "pscost_rt"
#define BRANCHRULE_DESC "Selects variables based on pscost. Either node (default) or edge variables."
#define BRANCHRULE_PRIORITY (100)
#define BRANCHRULE_MAXDEPTH (-1)
#define BRANCHRULE_MAXBOUNDDIST (1.0)

#define BRANCHRULE_EDGES false


BranchrulePscost::BranchrulePscost(SCIP *scip) : scip::ObjBranchrule(scip,
                                                                     BRANCHRULE_NAME,
                                                                     BRANCHRULE_DESC,
                                                                     BRANCHRULE_PRIORITY,
                                                                     BRANCHRULE_MAXDEPTH,
                                                                     BRANCHRULE_MAXBOUNDDIST),
                                                 branchOnEdges_(BRANCHRULE_EDGES) {
    SCIP_CALL_EXC_NO_THROW(
            SCIPaddBoolParam(scip,
                             "branching/" BRANCHRULE_NAME "/edges",
            "use edge-variables instead of node-variables for branching",
            &branchOnEdges_, FALSE, BRANCHRULE_EDGES, nullptr, nullptr))
}

/*
 * This is SCIPs pscost modified for node / edge branching
 */
SCIP_RETCODE BranchrulePscost::scip_execlp(SCIP *scip, SCIP_BRANCHRULE *branchrule, unsigned int /*allowaddcons*/,
                                           SCIP_RESULT *result) {
    SCIP_VAR **lpcands;
    SCIP_Real *lpcandssol;
    SCIP_Real bestscore;
    SCIP_Real bestrootdiff;
    int nlpcands;
    int bestcand;
    int c;

    assert(branchrule != nullptr);
    assert(scip != nullptr);
    assert(result != nullptr);

    /* get branching candidates */
    SCIP_CALL(SCIPgetLPBranchCands(scip, &lpcands, &lpcandssol, nullptr, nullptr, &nlpcands, nullptr));
    assert(nlpcands > 0);

    /* get probdata */
    auto *probData = SCIPgetObjProbData(scip_);
    auto probDataRootedMc = dynamic_cast<ProbDataRootedMc *>(probData);
    assert(probDataRootedMc != nullptr);

    bestcand = -1;
    bestscore = -SCIPinfinity(scip);
    bestrootdiff = 0.0;
    for (c = 0; c < nlpcands; ++c) {
        auto currentVar = lpcands[c];

        bool isNodeVar = true; //probDataRootedMc->isImplicitNodeVar(currentVar);
        bool isEdgeVar = true; //probDataRootedMc->isOriginalEdgeVar(currentVar);

        if ((branchOnEdges_ && isEdgeVar) || (!branchOnEdges_ && isNodeVar)) {
            auto currentVal = lpcandssol[c];

            SCIP_Real score;
            SCIP_Real rootsolval;
            SCIP_Real rootdiff;

            score = SCIPgetVarPseudocostScore(scip, currentVar, currentVal);
            rootsolval = SCIPvarGetRootSol(currentVar);
            rootdiff = REALABS(currentVal - rootsolval);
            if (SCIPisSumGT(scip, score, bestscore) ||
                (SCIPisSumEQ(scip, score, bestscore) && rootdiff > bestrootdiff)) {
                bestcand = c;
                bestscore = score;
                bestrootdiff = rootdiff;
            }
        }

    }
    assert(0 <= bestcand && bestcand < nlpcands);
    assert(!SCIPisFeasIntegral(scip, lpcandssol[bestcand]));
    assert(!SCIPisFeasIntegral(scip, SCIPvarGetSol(lpcands[bestcand], TRUE)));

    SCIP_CALL(SCIPbranchVar(scip, lpcands[bestcand], nullptr, nullptr, nullptr));
    std::cout << "Branching on " << SCIPvarGetName(lpcands[bestcand]) << std::endl;
    *result = SCIP_BRANCHED;

    return SCIP_OKAY;
}
