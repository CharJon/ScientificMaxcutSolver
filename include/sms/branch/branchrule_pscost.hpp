#ifndef SMS_BRANCHRULE_PSCOST_HPP
#define SMS_BRANCHRULE_PSCOST_HPP

#include "objscip/objscip.h"

class BranchrulePscost : public scip::ObjBranchrule {

private:
    SCIP_Bool branchOnEdges_ = false;

public:
    SCIP_RETCODE
    scip_execlp(SCIP *scip, SCIP_BRANCHRULE *branchrule, unsigned int allowaddcons, SCIP_RESULT *result) override;

    explicit BranchrulePscost(SCIP *scip);
};


#endif //SMS_BRANCHRULE_PSCOST_HPP
