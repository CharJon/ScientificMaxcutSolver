#include "sms/branch/branchrule_smallest_last.hpp"

#define BRANCHRULE_NAME "smallest_last"
#define BRANCHRULE_DESC "Selects the node based on smallest last ordering."
#define BRANCHRULE_PRIORITY 30000
#define BRANCHRULE_MAXDEPTH 30
#define BRANCHRULE_MAXBOUNDDIST 1.0

using NetworKit::node;
using NetworKit::count;
using NetworKit::Edge;
using NetworKit::Graph;

count BranchruleSmallestLast::degree(node u) const {
    unsigned int degree = 0;
    SCIP_VAR *nodeVar = model->nodeToTransVar(u);
    if (varIsFree(nodeVar)) {
        for (auto neighbor: model->graph->neighborRange(u)) {
            SCIP_VAR *neighbor_var = model->nodeToTransVar(neighbor);
            if (!dynamic_ || varIsFree(neighbor_var)) {
                degree++;
            }
        }
    }
    return degree;
}

std::vector<NetworKit::node> BranchruleSmallestLast::smallestLast() const {
    std::deque<node> smallestLastDeque;

    count M = std::numeric_limits<count>::max();
    auto degrees = std::vector<count>(model->graph->upperNodeIdBound(), M);

    count smallestDegree = model->graph->degree(0);
    count largestDegree = model->graph->degree(0);

    // collect degrees and setup buckets
    std::vector<std::vector<node>> degreeBuckets = {};
    auto nodeInBucketPosition = std::vector<count>(model->graph->upperNodeIdBound(), M);
    for (auto u: model->graph->nodeRange()) {
        auto currentDegree = degree(u);
        degrees[u] = currentDegree;

        smallestDegree = std::min(smallestDegree, currentDegree);
        largestDegree = std::max(largestDegree, currentDegree);

        while (degreeBuckets.size() <= largestDegree) {
            degreeBuckets.emplace_back();
        }

        nodeInBucketPosition[u] = degreeBuckets[currentDegree].size();
        degreeBuckets[currentDegree].push_back(u);
    }

    for (count i = 0; i < model->graph->numberOfNodes(); i++) {
        smallestDegree = 0;
        while (degreeBuckets[smallestDegree].empty()) {
            smallestDegree += 1;
        }

        node smallest = degreeBuckets[smallestDegree].back();
        degreeBuckets[smallestDegree].pop_back();
        degrees[smallest] = 0;

        smallestLastDeque.push_front(smallest);

        // update degrees
        for (node currentNeighbor: model->graph->neighborRange(smallest)) {
            SCIP_VAR *neighborVar = model->nodeToTransVar(currentNeighbor);
            if (dynamic_ && !varIsFree(neighborVar)) {
                continue;
            }
            // Check if neighbor still exists
            if (degrees[currentNeighbor] > 0) {
                auto oldDegree = degrees[currentNeighbor];
                auto newDegree = oldDegree - 1;
                auto oldPosition = nodeInBucketPosition[currentNeighbor];
                auto newPosition = degreeBuckets[newDegree].size();

                // Update for new degree
                degrees[currentNeighbor] = newDegree;
                nodeInBucketPosition[currentNeighbor] = newPosition;
                degreeBuckets[newDegree].push_back(currentNeighbor);

                // Delete for old degree
                node back = degreeBuckets[oldDegree].back();
                if (back != currentNeighbor) {
                    degreeBuckets[oldDegree][oldPosition] = back;
                    nodeInBucketPosition[back] = oldPosition;
                }
                degreeBuckets[oldDegree].pop_back();
            }
        }
    }

    assert(smallestDegree == 0);
    auto result = std::vector<node>(smallestLastDeque.begin(), smallestLastDeque.end());
    return result;

}

bool BranchruleSmallestLast::varIsFree(SCIP_VAR *neighborVar) {
    return (SCIPvarGetUbLocal(neighborVar) - SCIPvarGetLbLocal(neighborVar)) >= 0.5;
}

BranchruleSmallestLast::BranchruleSmallestLast(McExtendedModel *model, bool dynamic) : scip::ObjBranchrule(model->scip,
                                                                                                           BRANCHRULE_NAME,
                                                                                                           BRANCHRULE_DESC,
                                                                                                           BRANCHRULE_PRIORITY,
                                                                                                           BRANCHRULE_MAXDEPTH,
                                                                                                           BRANCHRULE_MAXBOUNDDIST),
                                                                                       dynamic_(dynamic) {

    this->model = model;
}


SCIP_RETCODE
BranchruleSmallestLast::scip_execlp(SCIP *scip, SCIP_BRANCHRULE * /*branchrule*/, unsigned int /*allowaddcons*/,
                                    SCIP_RESULT *result) {
    *result = SCIP_DIDNOTRUN;

    // get LP branching candidates
    int number_of_cands;
    SCIP_VAR **candidates;
    SCIP_Real *fractional_vals;
    SCIP_CALL(
            SCIPgetLPBranchCands(scip, &candidates, nullptr, &fractional_vals, nullptr, &number_of_cands, nullptr));

    auto smallestLastOrder = smallestLast();
    auto smallestLastPosition = std::vector<unsigned int>(model->graph->upperNodeIdBound(),
                                                          std::numeric_limits<unsigned int>::max());

    for (unsigned int i = 0; i < smallestLast().size(); i++) {
        auto u = smallestLastOrder[i];
        smallestLastPosition[u] = i;
    }

    unsigned int minPos = std::numeric_limits<unsigned int>::max();
    int bestCandIdx = -1;
    for (int i = 0; i < number_of_cands; i++) {
        SCIP_VAR *currentVar = candidates[i];
        assert(SCIPvarIsTransformedOrigvar(currentVar));
        if (model->varType(currentVar) == 'n') {
            int varId = model->varToId(currentVar);
            unsigned int pos = smallestLastPosition[varId];
            if (pos < minPos) {
                minPos = pos;
                bestCandIdx = i;
            }
        }
    }
    SCIP_NODE *downchild;
    SCIP_NODE *upchild;
    SCIP_NODE *eqchild;

    SCIP_VAR *bestVar = candidates[bestCandIdx];
    //std::cout << "Branching var pos: " << bestCandIdx << std::endl;
    // auto col = SCIPvarGetCol(bestVar);
    //std::cout << SCIPvarGetName(candidates[best_cand_idx]) << ": " << SCIPgetVarPseudocostScore(scip, bestVar,
    //                                                                                            SCIPvarGetSol(bestVar,
    //                                                                                                          true))
    //          << ". num rows of col: " << col->size << std::endl;
    SCIPbranchVar(model->scip,  bestVar, &downchild, &eqchild, &upchild);
    assert(downchild != nullptr);
    assert(upchild != nullptr);

    *result = SCIP_BRANCHED;

    return SCIP_OKAY;
}
