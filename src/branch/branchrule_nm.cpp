#include "sms/branch/branchrule_nm.hpp"

#include "sms/auxiliary/scip.hpp"
#include "sms/probdata/mc_rooted.hpp"

#define BRANCHRULE_NAME "nm"
#define BRANCHRULE_DESC "Suggested by Nguyen & Mineaux: Selects variables based on the number of binary neighbors."
#define BRANCHRULE_PRIORITY (300000)
#define BRANCHRULE_MAXDEPTH (-1)
#define BRANCHRULE_MAXBOUNDDIST (1.0)

#define BRANCHRULE_EDGES false


BranchruleNM::BranchruleNM(SCIP *scip) : scip::ObjBranchrule(scip,
                                                             BRANCHRULE_NAME,
                                                             BRANCHRULE_DESC,
                                                             BRANCHRULE_PRIORITY,
                                                             BRANCHRULE_MAXDEPTH,
                                                             BRANCHRULE_MAXBOUNDDIST) {}


int64_t BranchruleNM::degreeBinary(SCIP_VAR *variable) {
    auto *probData = SCIPgetObjProbData(scip_);
    auto probDataRootedMc = dynamic_cast<ProbDataRootedMc *>(probData);
    assert(probDataRootedMc != nullptr);

    auto root = probDataRootedMc->getRoot();
    auto u = probDataRootedMc->varToNode(variable);

    int64_t degree = 0;
    for (auto neighbor: probDataRootedMc->getMcGraph()->neighborRange(u)) {
        if (neighbor != root) {
            SCIP_VAR *neighborVar = probDataRootedMc->nodeToVar(neighbor);
            if (isBinary(scip_, neighborVar)) {
                degree++;
            }
        }
    }

    return degree;
}

SCIP_RETCODE BranchruleNM::scip_execlp(SCIP *scip, SCIP_BRANCHRULE */*branchrule*/, unsigned int /*allowaddcons*/,
                                       SCIP_RESULT *result) {
    *result = SCIP_DIDNOTRUN;

    // get LP branching candidates
    int number_of_cands;
    SCIP_VAR **candidates;
    SCIP_Real *fractional_vals;
    SCIP_CALL(
            SCIPgetLPBranchCands(scip, &candidates, nullptr, &fractional_vals, nullptr, &number_of_cands, nullptr));

    //uint64_t bestScore = 0;
    //int bestCandIdx = -1;

    int nmCandidates = 0;
    // average degree of graph
    auto *probData = SCIPgetObjProbData(scip_);
    auto probDataRootedMc = dynamic_cast<ProbDataRootedMc *>(probData);
    assert(probDataRootedMc != nullptr);

    unsigned int degreeSum = 0;
    for (auto u: probDataRootedMc->getMcGraph()->nodeRange()) {
        if (u != probDataRootedMc->getRoot()) {
            degreeSum += probDataRootedMc->getMcGraph()->degree(u);
        }
    }
    double avgDegree =
            static_cast<double>(degreeSum) / static_cast<double>(probDataRootedMc->getMcGraph()->numberOfNodes());
    // round avgDegree down to the nearest integer and save in an int type
    int avgDegreeInt = static_cast<int>(floor(avgDegree));

    int64_t maxBinaryNeighbors = 0;
    for (int i = 0; i < number_of_cands; i++) {
        SCIP_VAR *currentVar = candidates[i];

        int64_t binaryNeighbors = degreeBinary(currentVar);
        if (binaryNeighbors > maxBinaryNeighbors) {
            maxBinaryNeighbors = binaryNeighbors;
        }
        if (binaryNeighbors > avgDegreeInt) {
            nmCandidates++;
        }

    }

    // branch if number of binary neighbors is greater than average degree
    if (nmCandidates > 1) {
        std::cout << "NM branching found " << nmCandidates << " for NM branching: " << maxBinaryNeighbors << std::endl;
        // branch on variable with max number of binary neighbors
        // SCIP_CALL(SCIPbranchVar(scip, lpcands[maxPos], nullptr, nullptr, nullptr));
        //*result = SCIP_BRANCHED;
    } else {
        std::cout << "NM branching did not find, max was " << maxBinaryNeighbors << " and average is " << avgDegreeInt
                  << std::endl;
    }

    *result = SCIP_DIDNOTRUN;


    return SCIP_OKAY;
}
