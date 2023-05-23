#ifndef SMS_BRANCHRULEDEGREE_HPP
#define SMS_BRANCHRULEDEGREE_HPP

#include "objscip/objscip.h"

class BranchruleDegree : public scip::ObjBranchrule {

public:
    explicit BranchruleDegree(SCIP *scip, bool dynamic = false);

    SCIP_RETCODE
    scip_execlp(SCIP *scip, SCIP_BRANCHRULE *branchrule, unsigned int allowaddcons, SCIP_RESULT *result) override;

private:
    SCIP_Bool dynamic_ = false;

    uint64_t degree(SCIP_VAR *variable);

};

#endif // SMS_BRANCHRULEDEGREE_HPP
