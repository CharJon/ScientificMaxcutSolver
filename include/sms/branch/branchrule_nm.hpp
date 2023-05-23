#ifndef SMS_BRANCHRULE_NM_HPP
#define SMS_BRANCHRULE_NM_HPP

#include "objscip/objscip.h"

class BranchruleNM : public scip::ObjBranchrule {

public:
    SCIP_RETCODE
    scip_execlp(SCIP *scip, SCIP_BRANCHRULE *branchrule, unsigned int allowaddcons, SCIP_RESULT *result) override;

    explicit BranchruleNM(SCIP *scip);

private:
    uint64_t numBinaryNeighbors(SCIP_VAR *variable);

    int64_t degreeBinary(SCIP_VAR *variable);
};


#endif //SMS_BRANCHRULE_NM_HPP
