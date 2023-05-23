#include <utility>

#include "sms/eventhdlr/eventhdlr_root.hpp"

#include "sms/auxiliary/scip.hpp"

#define EVENTHDLR_NAME         "root-statistics"
#define EVENTHDLR_DESC         "event handler for root statistics"

#define EVENT (SCIP_EVENTTYPE_NODESOLVED)


EventhdlrRoot::EventhdlrRoot(SCIP *scip, std::string filePath)
        : ObjEventhdlr(scip, EVENTHDLR_NAME, EVENTHDLR_DESC),
          filePath_(std::move(filePath)) {}

/** destructor of event handler to free user data (called when SCIP is exiting) */
SCIP_RETCODE EventhdlrRoot::scip_free(SCIP */*scip*/, SCIP_EVENTHDLR */*eventhdlr*/) {
    return SCIP_OKAY;
}


/** initialization method of event handler (called after problem was transformed) */
SCIP_RETCODE EventhdlrRoot::scip_init(SCIP */*scip*/, SCIP_EVENTHDLR */*eventhdlr*/) {
    return SCIP_OKAY;
}


/** deinitialization method of event handler (called before transformed problem is freed) */
SCIP_RETCODE EventhdlrRoot::scip_exit(SCIP */*scip*/, SCIP_EVENTHDLR */*eventhdlr*/) {
    return SCIP_OKAY;
}


/** solving process initialization method of event handler (called when branch and bound process is about to begin)
 *
 *  This method is called when the presolving was finished and the branch and bound process is about to begin.
 *  The event handler may use this call to initialize its branch and bound specific data.
 *
 */
SCIP_RETCODE EventhdlrRoot::scip_initsol(SCIP *scip, SCIP_EVENTHDLR *eventhdlr) {
    SCIP_CALL(SCIPcatchEvent(scip, EVENT, eventhdlr, nullptr, nullptr));
    return SCIP_OKAY;
}


/** solving process deinitialization method of event handler (called before branch and bound process data is freed)
 *
 *  This method is called before the branch and bound process is freed.
 *  The event handler should use this call to clean up its branch and bound data.
 */
SCIP_RETCODE EventhdlrRoot::scip_exitsol(SCIP */*scip*/, SCIP_EVENTHDLR */*eventhdlr*/) {
    return SCIP_OKAY;
}


/** frees specific constraint data */
SCIP_RETCODE
EventhdlrRoot::scip_delete(SCIP */*scip*/, SCIP_EVENTHDLR */*eventhdlr*/, SCIP_EVENTDATA **/*eventdata*/) {
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
EventhdlrRoot::scip_exec(SCIP *scip, SCIP_EVENTHDLR *eventhdlr, SCIP_EVENT */*event*/,
                         SCIP_EVENTDATA */*eventdata*/) {
    if (!rootDone_) {
        printScipStatistics(scip, filePath_);
    }
    rootDone_ = true;
    SCIP_CALL(SCIPdropEvent(scip, EVENT, eventhdlr, nullptr, -1));
    return SCIP_OKAY;
}

