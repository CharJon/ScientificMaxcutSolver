#ifndef SMS_BRANCHRULESMALLESTLAST_HPP
#define SMS_BRANCHRULESMALLESTLAST_HPP

#include "objscip/objscip.h"

#include "sms/auxiliary/mc_extended_model.hpp"

class BranchruleSmallestLast : public scip::ObjBranchrule {
    McExtendedModel *model;

    //    SCIP_RETCODE scip_free(SCIP *scip, SCIP_BRANCHRULE *branchrule) override;

private:
    bool dynamic_ = false;

    NetworKit::count degree(NetworKit::node u) const;

    std::vector<NetworKit::node> smallestLast() const;

public:
    SCIP_RETCODE
    scip_execlp(SCIP *scip, SCIP_BRANCHRULE *branchrule, unsigned int allowaddcons, SCIP_RESULT *result) override;

    explicit BranchruleSmallestLast(McExtendedModel *model, bool dynamic = false);

    static bool varIsFree(SCIP_VAR *neighborVar) ;
};

#endif // SMS_BRANCHRULESMALLESTLAST_HPP
