#ifndef SMS_CONSHDLRTRIANGLES_HPP
#define SMS_CONSHDLRTRIANGLES_HPP

#include <iostream>
#include <utility>
#include <chrono>

#include "objscip/objscip.h"
#include "networkit/graph/Graph.hpp"
#include "sms/conshdlr/triangles.hpp"


using NetworKit::node;
using NetworKit::Edge;
using NetworKit::Graph;

/** C++ constraint handler for triangles (on edge variables) */
class ConshdlrTriangles : public scip::ObjConshdlr {

private:
    Graph const *const sepGraph_;
    TriangleSeparator triangleSeparator_;
    SCIP_Bool sepaCutpool_;

public:

    /** constructor */
    explicit ConshdlrTriangles(Scip *scip, const Graph *sepGraph);


    /** destructor */
    ~ConshdlrTriangles() override = default;



    /****
     * FUNDAMENTAL Callbacks as of SCIP Documentation
     * CONSCHECK (global feasibility of primal solution)
     * CONSENFOLP (after price-and-cut for lp solution feasibility)
     * CONSENFOPS (similar to above CONSENFOLP but for pseudo solutions)
     * CONSENFORELAX (similar to above CONSENFOLP but for relaxation solutions)
     * CONSLOCK
     ****/

    /** feasibility check method of constraint handler for primal solutions
     *
     *  The given solution has to be checked for feasibility.
     *
     *  The check methods of the active constraint handlers are called in decreasing order of their check
     *  priorities until the first constraint handler returned with the result SCIP_INFEASIBLE.
     *  The integrality constraint handler has a check priority of zero. A constraint handler which can
     *  (or wants) to check its constraints only for integral solutions should have a negative check priority
     *  (e.g. the alldiff-constraint can only operate on integral solutions).
     *  A constraint handler which wants to check feasibility even on non-integral solutions must have a
     *  check priority greater than zero (e.g. if the check is much faster than testing all variables for
     *  integrality).
     *
     *  In some cases, integrality conditions or rows of the current LP don't have to be checked, because their
     *  feasibility is already checked or implicitly given. In these cases, 'checkintegrality' or
     *  'checklprows' is FALSE.
     *
     *  possible return values for *result:
     *  - SCIP_INFEASIBLE : at least one constraint of the handler is infeasible
     *  - SCIP_FEASIBLE   : all constraints of the handler are feasible
     */
    SCIP_DECL_CONSCHECK(scip_check) override;

    /** constraint enforcing method of constraint handler for LP solutions
     *
     *  The method is called at the end of the node processing loop for a node where the LP was solved.
     *  The LP solution has to be checked for feasibility. If possible, an infeasibility should be resolved by
     *  branching, reducing a variable's domain to exclude the solution or separating the solution with a valid
     *  cutting plane.
     *
     *  The enforcing methods of the active constraint handlers are called in decreasing order of their enforcing
     *  priorities until the first constraint handler returned with the value SCIP_CUTOFF, SCIP_SEPARATED,
     *  SCIP_REDUCEDDOM, SCIP_CONSADDED, or SCIP_BRANCHED.
     *  The integrality constraint handler has an enforcing priority of zero. A constraint handler which can
     *  (or wants) to enforce its constraints only for integral solutions should have a negative enforcing priority
     *  (e.g. the alldiff-constraint can only operate on integral solutions).
     *  A constraint handler which wants to incorporate its own branching strategy even on non-integral
     *  solutions must have an enforcing priority greater than zero (e.g. the SOS-constraint incorporates
     *  SOS-branching on non-integral solutions).
     *
     *  The first nusefulconss constraints are the ones, that are identified to likely be violated. The enforcing
     *  method should process the useful constraints first. The other nconss - nusefulconss constraints should only
     *  be enforced, if no violation was found in the useful constraints.
     *
     *  possible return values for *result (if more than one applies, the first in the list should be used):
     *  - SCIP_CUTOFF     : the node is infeasible in the variable's bounds and can be cut off
     *  - SCIP_CONSADDED  : an additional constraint was generated
     *  - SCIP_REDUCEDDOM : a variable's domain was reduced
     *  - SCIP_SEPARATED  : a cutting plane was generated
     *  - SCIP_BRANCHED   : no changes were made to the problem, but a branching was applied to resolve an infeasibility
     *  - SCIP_INFEASIBLE : at least one constraint is infeasible, but it was not resolved
     *  - SCIP_FEASIBLE   : all constraints of the handler are feasible
     */
    SCIP_DECL_CONSENFOLP(scip_enfolp) override;

    /** constraint enforcing method of constraint handler for pseudo solutions
     *
     *  The method is called at the end of the node processing loop for a node where the LP was not solved.
     *  The pseudo solution has to be checked for feasibility. If possible, an infeasibility should be resolved by
     *  branching, reducing a variable's domain to exclude the solution or adding an additional constraint.
     *  Separation is not possible, since the LP is not processed at the current node. All LP informations like
     *  LP solution, slack values, or reduced costs are invalid and must not be accessed.
     *
     *  Like in the enforcing method for LP solutions, the enforcing methods of the active constraint handlers are
     *  called in decreasing order of their enforcing priorities until the first constraint handler returned with
     *  the value SCIP_CUTOFF, SCIP_REDUCEDDOM, SCIP_CONSADDED, SCIP_BRANCHED, or SCIP_SOLVELP.
     *
     *  The first nusefulconss constraints are the ones, that are identified to likely be violated. The enforcing
     *  method should process the useful constraints first. The other nconss - nusefulconss constraints should only
     *  be enforced, if no violation was found in the useful constraints.
     *
     *  If the pseudo solution's objective value is lower than the lower bound of the node, it cannot be feasible
     *  and the enforcing method may skip it's check and set *result to SCIP_DIDNOTRUN. However, it can also process
     *  its constraints and return any other possible result code.
     *
     *  possible return values for *result (if more than one applies, the first in the list should be used):
     *  - SCIP_CUTOFF     : the node is infeasible in the variable's bounds and can be cut off
     *  - SCIP_CONSADDED  : an additional constraint was generated
     *  - SCIP_REDUCEDDOM : a variable's domain was reduced
     *  - SCIP_BRANCHED   : no changes were made to the problem, but a branching was applied to resolve an infeasibility
     *  - SCIP_SOLVELP    : at least one constraint is infeasible, and this can only be resolved by solving the SCIP_LP
     *  - SCIP_INFEASIBLE : at least one constraint is infeasible, but it was not resolved
     *  - SCIP_FEASIBLE   : all constraints of the handler are feasible
     *  - SCIP_DIDNOTRUN  : the enforcement was skipped (only possible, if objinfeasible is true)
     */
    SCIP_DECL_CONSENFOPS(scip_enfops) override;

    /** variable rounding lock method of constraint handler
     *
     *  This method is called, after a constraint is added or removed from the transformed problem.
     *  It should update the rounding locks of all associated variables with calls to SCIPaddVarLocksType(),
     *  depending on the way, the variable is involved in the constraint:
     *  - If the constraint may get violated by decreasing the value of a variable, it should call
     *    SCIPaddVarLocksType(scip, var, SCIP_LOCKTYPE_MODEL, nlockspos, nlocksneg), saying that rounding down is
     *    potentially rendering the (positive) constraint infeasible and rounding up is potentially rendering the
     *    negation of the constraint infeasible.
     *  - If the constraint may get violated by increasing the value of a variable, it should call
     *    SCIPaddVarLocksType(scip, var, SCIP_LOCKTYPE_MODEL, nlocksneg, nlockspos), saying that rounding up is
     *    potentially rendering the constraint's negation infeasible and rounding up is potentially rendering the
     *    constraint itself infeasible.
     *  - If the constraint may get violated by changing the variable in any direction, it should call
     *    SCIPaddVarLocksType(scip, var, SCIP_LOCKTYPE_MODEL, nlockspos + nlocksneg, nlockspos + nlocksneg).
     *
     *  Consider the linear constraint "3x -5y +2z <= 7" as an example. The variable rounding lock method of the
     *  linear constraint handler should call SCIPaddVarLocksType(scip, x, SCIP_LOCKTYPE_MODEL, nlocksneg, nlockspos),
     *  SCIPaddVarLocksType(scip, y, SCIP_LOCKTYPE_MODEL, nlockspos, nlocksneg) and
     *  SCIPaddVarLocksType(scip, z, SCIP_LOCKTYPE_MODEL, nlocksneg, nlockspos) to tell SCIP,
     *  that rounding up of x and z and rounding down of y can destroy the feasibility of the constraint, while rounding
     *  down of x and z and rounding up of y can destroy the feasibility of the constraint's negation "3x -5y +2z > 7".
     *  A linear constraint "2 <= 3x -5y +2z <= 7" should call
     *  SCIPaddVarLocksType(scip, ..., SCIP_LOCKTYPE_MODEL, nlockspos + nlocksneg, nlockspos + nlocksneg) on all variables,
     *  since rounding in both directions of each variable can destroy both the feasibility of the constraint and it's negation
     *  "3x -5y +2z < 2  or  3x -5y +2z > 7".
     *
     *  If the constraint itself contains other constraints as sub constraints (e.g. the "or" constraint concatenation
     *  "c(x) or d(x)"), the rounding lock methods of these constraints should be called in a proper way.
     *  - If the constraint may get violated by the violation of the sub constraint c, it should call
     *    SCIPaddConsLocks(scip, c, nlockspos, nlocksneg), saying that infeasibility of c may lead to infeasibility of
     *    the (positive) constraint, and infeasibility of c's negation (i.e. feasibility of c) may lead to infeasibility
     *    of the constraint's negation (i.e. feasibility of the constraint).
     *  - If the constraint may get violated by the feasibility of the sub constraint c, it should call
     *    SCIPaddConsLocks(scip, c, nlocksneg, nlockspos), saying that infeasibility of c may lead to infeasibility of
     *    the constraint's negation (i.e. feasibility of the constraint), and infeasibility of c's negation (i.e. feasibility
     *    of c) may lead to infeasibility of the (positive) constraint.
     *  - If the constraint may get violated by any change in the feasibility of the sub constraint c, it should call
     *    SCIPaddConsLocks(scip, c, nlockspos + nlocksneg, nlockspos + nlocksneg).
     *
     *  Consider the or concatenation "c(x) or d(x)". The variable rounding lock method of the or constraint handler
     *  should call SCIPaddConsLocks(scip, c, nlockspos, nlocksneg) and SCIPaddConsLocks(scip, d, nlockspos, nlocksneg)
     *  to tell SCIP, that infeasibility of c and d can lead to infeasibility of "c(x) or d(x)".
     *
     *  As a second example, consider the equivalence constraint "y <-> c(x)" with variable y and constraint c. The
     *  constraint demands, that y == 1 if and only if c(x) is satisfied. The variable lock method of the corresponding
     *  constraint handler should call SCIPaddVarLocksType(scip, y, SCIP_LOCKTYPE_MODEL, nlockspos + nlocksneg, nlockspos + nlocksneg) and
     *  SCIPaddConsLocks(scip, c, nlockspos + nlocksneg, nlockspos + nlocksneg), because any modification to the
     *  value of y or to the feasibility of c can alter the feasibility of the equivalence constraint.
     */
    SCIP_DECL_CONSLOCK(scip_lock) override;

    /****
     * NON FUNDAMENTAL Callbacks as of SCIP Documentation
     ****/

    /** transforms constraint data into data belonging to the transformed problem */
    // Fully virtual, therefore needs to be implemented for project to compile, but docu states otherwise
    SCIP_DECL_CONSTRANS(scip_trans) override;

    /** clone method which will be used to copy a objective plugin */
    SCIP_DECL_CONSHDLRCLONE(scip::ObjProbCloneable *clone) override;

    /** separation method of constraint handler for arbitrary primal solution
     *
     *  Separates all constraints of the constraint handler. The method is called outside the LP solution loop (e.g., by
     *  a relaxator or a primal heuristic), which means that there is no valid LP solution.
     *  Instead, the method should produce cuts that separate the given solution.
     *
     *  The first nusefulconss constraints are the ones, that are identified to likely be violated. The separation
     *  method should process only the useful constraints in most runs, and only occasionally the remaining
     *  nconss - nusefulconss constraints.
     *
     *  possible return values for *result (if more than one applies, the first in the list should be used):
     *  - SCIP_CUTOFF     : the node is infeasible in the variable's bounds and can be cut off
     *  - SCIP_CONSADDED  : an additional constraint was generated
     *  - SCIP_REDUCEDDOM : a variable's domain was reduced
     *  - SCIP_SEPARATED  : a cutting plane was generated
     *  - SCIP_DIDNOTFIND : the separator searched, but did not find domain reductions, cutting planes, or cut constraints
     *  - SCIP_DIDNOTRUN  : the separator was skipped
     *  - SCIP_DELAYED    : the separator was skipped, but should be called again
     */
    virtual SCIP_DECL_CONSSEPASOL(scip_sepasol);

    /** separation method of constraint handler for LP solution
     *
     *  Separates all constraints of the constraint handler. The method is called in the LP solution loop,
     *  which means that a valid LP solution exists.
     *
     *  The first nusefulconss constraints are the ones, that are identified to likely be violated. The separation
     *  method should process only the useful constraints in most runs, and only occasionally the remaining
     *  nconss - nusefulconss constraints.
     *
     *  possible return values for *result (if more than one applies, the first in the list should be used):
     *  - SCIP_CUTOFF     : the node is infeasible in the variable's bounds and can be cut off
     *  - SCIP_CONSADDED  : an additional constraint was generated
     *  - SCIP_REDUCEDDOM : a variable's domain was reduced
     *  - SCIP_SEPARATED  : a cutting plane was generated
     *  - SCIP_DIDNOTFIND : the separator searched, but did not find domain reductions, cutting planes, or cut constraints
     *  - SCIP_DIDNOTRUN  : the separator was skipped
     *  - SCIP_DELAYED    : the separator was skipped, but should be called again
     */
    virtual SCIP_DECL_CONSSEPALP(scip_sepalp);

    virtual SCIP_DECL_CONSINITSOL(scip_initsol);

private:

    /** separates odd cycle cuts */
    SCIP_RETCODE sepaTrianglesLp(
            SCIP *scip,               /**< SCIP data structure */
            SCIP_CONSHDLR *conshdlr,  /**< the constraint handler itself */
            SCIP_CONS **/*conss*/,        /**< array of constraints to process */
            int /*nconss*/,               /**< number of constraints to process */
            int /*nusefulconss*/,         /**< number of useful (non-obsolete) constraints to process */
            SCIP_RESULT *result
    );

};

/** creates and captures an OddCycle-MaxCut constraint */
SCIP_RETCODE SCIPcreateConsTriangles(
        SCIP *scip,                   /**< SCIP data structure */
        SCIP_CONS **cons,             /**< pointer to hold the created constraint */
        const char *name,             /**< name of constraint */
        const Graph *graph,           /**< the graph to separate on */
        SCIP_Bool initial,            /**< should the LP relaxation of constraint be in the initial LP? */
        SCIP_Bool separate,           /**< should the constraint be separated during LP processing? */
        SCIP_Bool enforce,            /**< should the constraint be enforced during node processing? */
        SCIP_Bool check,              /**< should the constraint be checked for feasibility? */
        SCIP_Bool propagate,          /**< should the constraint be propagated during node processing? */
        SCIP_Bool local,              /**< is constraint only valid locally? */
        SCIP_Bool modifiable,         /**< is constraint modifiable (subject to column generation)? */
        SCIP_Bool dynamic,            /**< is constraint dynamic? */
        SCIP_Bool removable           /**< should the constraint be removed from the LP due to aging or cleanup? */
);

/** creates and captures an OddCycle-MaxCut constraint */
SCIP_RETCODE SCIPcreateConsBasicTriangles(
        SCIP *scip,                   /**< SCIP data structure */
        SCIP_CONS **cons,             /**< pointer to hold the created constraint */
        const char *name,             /**< name of constraint */
        const Graph *graph           /**< the graph to separate on */
);


#endif //SMS_CONSHDLRTRIANGLES_HPP
