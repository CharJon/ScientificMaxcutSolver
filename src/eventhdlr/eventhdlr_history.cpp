#include "sms/eventhdlr/eventhdlr_history.hpp"

#include <iostream>
#include "nlohmann/json.hpp"
#include "objscip/objscip.h"

#define EVENTHDLR_NAME         "bounds-history"
#define EVENTHDLR_DESC         "event handler for bounds history"

#define EVENT (SCIP_EVENTTYPE_LPSOLVED | SCIP_EVENTTYPE_BESTSOLFOUND | SCIP_EVENTTYPE_NODEEVENT)


EventhdlrHistory::EventhdlrHistory(SCIP *scip)
        : ObjEventhdlr(scip, EVENTHDLR_NAME, EVENTHDLR_DESC),
          primalRoot_(SCIPinfinity(scip)),
          dualRoot_(SCIPinfinity(scip)),
          gapRoot_(SCIPinfinity(scip)) {}

/** destructor of event handler to free user data (called when SCIP is exiting) */
SCIP_RETCODE EventhdlrHistory::scip_free(SCIP */*scip*/, SCIP_EVENTHDLR */*eventhdlr*/) {
    return SCIP_OKAY;
}


/** initialization method of event handler (called after problem was transformed) */
SCIP_RETCODE EventhdlrHistory::scip_init(SCIP */*scip*/, SCIP_EVENTHDLR */*eventhdlr*/) {
    return SCIP_OKAY;
}


/** deinitialization method of event handler (called before transformed problem is freed) */
SCIP_RETCODE EventhdlrHistory::scip_exit(SCIP */*scip*/, SCIP_EVENTHDLR */*eventhdlr*/) {
    return SCIP_OKAY;
}


/** solving process initialization method of event handler (called when branch and bound process is about to begin)
 *
 *  This method is called when the presolving was finished and the branch and bound process is about to begin.
 *  The event handler may use this call to initialize its branch and bound specific data.
 *
 */
SCIP_RETCODE EventhdlrHistory::scip_initsol(SCIP *scip, SCIP_EVENTHDLR *eventhdlr) {
    SCIP_CALL(SCIPcatchEvent(scip, EVENT, eventhdlr, nullptr, nullptr));

    return SCIP_OKAY;
}


/** solving process deinitialization method of event handler (called before branch and bound process data is freed)
 *
 *  This method is called before the branch and bound process is freed.
 *  The event handler should use this call to clean up its branch and bound data.
 */
SCIP_RETCODE EventhdlrHistory::scip_exitsol(SCIP *scip, SCIP_EVENTHDLR *eventhdlr) {
    SCIP_CALL(SCIPdropEvent(scip, EVENT, eventhdlr, nullptr, -1));

    return SCIP_OKAY;
}


/** frees specific constraint data */
SCIP_RETCODE
EventhdlrHistory::scip_delete(SCIP */*scip*/, SCIP_EVENTHDLR */*eventhdlr*/, SCIP_EVENTDATA **/*eventdata*/) {
    return SCIP_OKAY;
}


/** execution method of event handler
 *
 *  Processes the event. The method is called every time an event occurs, for which the event handler
 *  is responsible. Event handlers may declare themselves responsible for events by calling the
 *  corresponding SCIPcatch...() method. This method creates an event filter object to point to the
 *  given event handler and event data.
 */
SCIP_RETCODE
EventhdlrHistory::scip_exec(SCIP *scip, SCIP_EVENTHDLR */*eventhdlr*/, SCIP_EVENT */*event*/,
                            SCIP_EVENTDATA */*eventdata*/) {
    if (!rootDone_) {
        primalRoot_ = SCIPgetPrimalbound(scip);
        dualRoot_ = SCIPgetDualbound(scip);
        gapRoot_ = SCIPgetGap(scip);
        rootDone_ = (SCIPgetNTotalNodes(scip) > 1);
    }

    times_.push_back(SCIPgetSolvingTime(scip));
    currentNodes_.push_back(SCIPgetNTotalNodes(scip));
    dualBounds_.push_back(SCIPgetDualbound(scip));
    primalBounds_.push_back(SCIPgetPrimalbound(scip));
    lpIterations_.push_back(SCIPgetNLPIterations(scip));

    return SCIP_OKAY;
}


void EventhdlrHistory::to_json(std::ostream &ostream) {
    nlohmann::json j;

    j["root"]["primal_bound"] = primalRoot_;
    j["root"]["dual_bound"] = dualRoot_;
    j["root"]["gap"] = gapRoot_;

    j["cpu_solving_time"] = times_;
    j["bab_nodes"] = currentNodes_;
    j["primalbound"] = primalBounds_;
    j["dualbound"] = dualBounds_;
    j["lp_iterations"] = lpIterations_;

    ostream << std::setw(4) << j << std::endl;
}
