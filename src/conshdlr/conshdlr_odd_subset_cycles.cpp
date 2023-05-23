#include "sms/conshdlr/conshdlr_odd_subset_cycles.hpp"

#include <cassert>
#include <iostream>
#include "networkit/graph/Graph.hpp"
#include "scip/cons_linear.h"
#include "objscip/objscip.h"

/* constraint handler properties */
#define CONSHDLR_NAME          "cycles"
#define CONSHDLR_DESC          "cycles for odd-subset cycle constraints"
#define CONSHDLR_SEPAPRIORITY   (+700000) /**< priority of the constraint handler for separation */
#define CONSHDLR_ENFOPRIORITY   (-600000) /**< priority of the constraint handler for constraint enforcing */
#define CONSHDLR_CHECKPRIORITY  (-600000) /**< priority of the constraint handler for checking feasibility */
#define CONSHDLR_SEPAFREQ             1 /**< frequency for separating cuts; zero means to separate only in the root node */
#define CONSHDLR_PROPFREQ             1 /**< frequency for propagating domains; zero means only preprocessing propagation */
#define CONSHDLR_EAGERFREQ          100 /**< frequency for using all instead of only the useful constraints in separation,
                                         *   propagation and enforcement, -1 for no eager evaluations, 0 for first only */
#define CONSHDLR_MAXPREROUNDS        (-1) /**< maximal number of presolving rounds the constraint handler participates in (-1: no limit) */
#define CONSHDLR_DELAYSEPA        FALSE /**< should separation method be delayed, if other separators found cuts? */
#define CONSHDLR_DELAYPROP        FALSE /**< should propagation method be delayed, if other propagators found reductions? */
#define CONSHDLR_NEEDSCONS         TRUE /**< should the constraint handler be skipped, if no constraints are available? */
#define CONSHDLR_PROP_TIMING             SCIP_PROPTIMING_BEFORELP
#define CONSHDLR_PRESOLTIMING            SCIP_PRESOLTIMING_ALWAYS

#define CONSHDLR_ENFORCE_CONS_DEFAULT TRUE


ConshdlrOddSubsetCycles::ConshdlrOddSubsetCycles(Scip *scip) : scip::ObjConshdlr(scip,
                                                                                 CONSHDLR_NAME,
                                                                                 CONSHDLR_DESC,
                                                                                 CONSHDLR_SEPAPRIORITY,
                                                                                 CONSHDLR_ENFOPRIORITY,
                                                                                 CONSHDLR_CHECKPRIORITY,
                                                                                 CONSHDLR_SEPAFREQ,
                                                                                 CONSHDLR_PROPFREQ,
                                                                                 CONSHDLR_EAGERFREQ,
                                                                                 CONSHDLR_MAXPREROUNDS,
                                                                                 CONSHDLR_DELAYSEPA,
                                                                                 CONSHDLR_DELAYPROP,
                                                                                 CONSHDLR_NEEDSCONS,
                                                                                 CONSHDLR_PROP_TIMING,
                                                                                 CONSHDLR_PRESOLTIMING),
                                                               enforceCons_(CONSHDLR_ENFORCE_CONS_DEFAULT) {
    SCIP_CALL_ABORT(SCIPaddBoolParam(scip,
                                     "constraints/" CONSHDLR_NAME "/enforce",
            "should constraints be enforced (otherwise they only get added to the cutpool)",
            &enforceCons_,
            FALSE,
            CONSHDLR_ENFORCE_CONS_DEFAULT,
            FALSE,
            nullptr));
}

/** data structure for odd-subset cycle constraints */
struct SCIP_ConsData {
    SCIP_ROW *row;        /**< LP row, if constraint is already stored in LP row format */
    SCIP_VAR **vars;      /**< variables of the constraint */
    int varssize;         /**< size of vars array */
    int nvars;            /**< number of variables in the constraint */
    SCIP_Real *currentCoefs;
    SCIP_Real currentOddSubsetSize;
};

SCIP_RETCODE consdataCreate(
        SCIP *scip,               /**< SCIP data structure */
        SCIP_CONSDATA **consdata,           /**< pointer to store the logic or constraint data */
        const std::vector<SCIP_VAR *> &vars       /**< variables of the constraint */
) {
    assert(scip != NULL);
    assert(consdata != NULL);
    assert(vars.size() >= 3);

    SCIP_CALL(SCIPallocBlockMemory(scip, consdata));

    (*consdata)->row = nullptr;
    (*consdata)->varssize = vars.size();
    (*consdata)->nvars = vars.size();

    SCIP_CALL(SCIPallocBlockMemoryArray(scip, &(*consdata)->vars, (*consdata)->varssize));

    for (int i = 0; i < (*consdata)->nvars; ++i) {
        assert(vars[i] != nullptr);
        (*consdata)->vars[i] = vars[i];
        SCIP_CALL(SCIPcaptureVar(scip, (*consdata)->vars[i]));
    }

    return SCIP_OKAY;
}

/** Use this to add a constraint to a model */
SCIP_RETCODE SCIPcreateConsCycle(
        SCIP *scip,               /**< SCIP data structure */
        SCIP_CONS **cons,               /**< pointer to hold the created constraint */
        const char *name,               /**< name of constraint */
        const std::vector<SCIP_VAR *> &vars,   /**< variables of constraint */
        SCIP_Bool initial,            /**< should the LP relaxation of constraint be in the initial LP? */
        SCIP_Bool separate,           /**< should the constraint be separated during LP processing? */
        SCIP_Bool enforce,            /**< should the constraint be enforced during node processing? */
        SCIP_Bool check,              /**< should the constraint be checked for feasibility? */
        SCIP_Bool propagate,          /**< should the constraint be propagated during node processing? */
        SCIP_Bool local,              /**< is constraint only valid locally? */
        SCIP_Bool modifiable,         /**< is constraint modifiable (subject to column generation)? */
        SCIP_Bool dynamic,            /**< is constraint dynamic? */
        SCIP_Bool removable           /**< should the constraint be removed from the LP due to aging or cleanup? */
) {
    SCIP_CONSHDLR *conshdlr;
    SCIP_ConsData *consdata;

    /* find correct constraint handler */
    conshdlr = SCIPfindConshdlr(scip, CONSHDLR_NAME);
    if (conshdlr == nullptr) {
        SCIPerrorMessage("constraint handler not found\n");
        return SCIP_PLUGINNOTFOUND;
    }

    /* create the constraint specific data */
    SCIP_CALL(consdataCreate(scip, &consdata, vars));

    /* create constraint */
    SCIP_CALL(SCIPcreateCons(scip, cons, name, conshdlr, consdata, initial, separate, enforce, check, propagate,
                             local, modifiable, dynamic, removable, FALSE));

    return SCIP_OKAY;
}

/** creates and captures an odd-subset max-cut constraint */
SCIP_RETCODE SCIPcreateConsBasicCycle(
        SCIP *scip,                   /**< SCIP data structure */
        SCIP_CONS **cons,             /**< pointer to hold the created constraint */
        const char *name,             /**< name of constraint */
        const std::vector<SCIP_VAR *> &vars   /**< variables of constraint */
) {
    return SCIPcreateConsCycle(scip, cons, name, vars, FALSE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE);
}

bool isConsViolated(
        SCIP *scip,               /**< SCIP data structure */
        SCIP_CONS *cons,          /**< logic or constraint to be checked */
        SCIP_SOL *sol             /**< primal SCIP solution or null for lp solution */
) {
    SCIP_CONSDATA *consdata = SCIPconsGetData(cons);
    assert(consdata != NULL);

    SCIP_VAR **vars = consdata->vars;
    int numVars = consdata->nvars;

    SCIP_Real sum = 0.0;
    std::vector<int8_t> subsets(numVars, 0);
    int oddSubsetSize = 0;
    SCIP_Real largestSmall = 0;
    int largestSmallPos = -1;
    SCIP_Real smallestLarge = 1;
    int smallestLargePos = -1;

    for (int v = 0; v < numVars; v++) {
        SCIP_Real solval = SCIPgetSolVal(scip, sol, vars[v]);
        assert(SCIPisFeasGE(scip, solval, 0.0) && SCIPisFeasLE(scip, solval, 1.0));
        if (solval < 0.5) {
            sum += solval;
            subsets[v] = -1;
            if (largestSmall < solval) {
                largestSmall = solval;
                largestSmallPos = v;
            }
        } else {
            sum += 1.0 - solval;
            oddSubsetSize++;
            subsets[v] = 1;
            if (smallestLarge > solval) {
                smallestLarge = solval;
                smallestLargePos = v;
            }
        }
    }

    assert(std::all_of(subsets.begin(), subsets.end(), [](int8_t i) { return i == -1 || i == 1; }));
    assert(std::count(subsets.begin(), subsets.end(), 1) == oddSubsetSize);

    if (SCIPisFeasGE(scip, sum, 1.0)) {
        return false;
    } else {
        bool isViolated = (oddSubsetSize % 2 == 1);

        // try moving best candidate from the odd-subset to the other subset
        if (!isViolated && SCIPisLT(scip, smallestLarge, 1)) {
            SCIP_Real alternativeSum = sum - 1 - smallestLarge - smallestLarge;
            if (SCIPisLT(scip, alternativeSum, 1.0)) {
                isViolated = true;
                sum = alternativeSum;
                subsets[smallestLargePos] = -1;
                oddSubsetSize -= 1;
            }
        }

        // try moving best candidate from the other subset to the odd subset
        if (!isViolated && SCIPisGT(scip, largestSmall, 0)) {
            SCIP_Real alternativeSum = sum + 1 + largestSmall + largestSmall;
            if (SCIPisLT(scip, alternativeSum, 1.0)) {
                isViolated = true;
                sum = alternativeSum;
                subsets[largestSmallPos] = 1;
                oddSubsetSize += 1;
            }
        }

        /* calculate constraint violation and update it in solution */
        if (isViolated && sol != nullptr) {
            SCIP_Real absviol = 1.0 - sum;
            SCIP_Real relviol = SCIPrelDiff(1.0, sum);
            SCIPupdateSolLPConsViolation(scip, sol, absviol, relviol);
        }

        return isViolated;
    }
}

/** creates an LP row in a logic or constraint data object */
SCIP_RETCODE createRow(
        SCIP *scip,              /**< SCIP data structure */
        SCIP_CONS *cons,         /**< logic or constraint */
        SCIP_Real *coefs,        /**< coefficients of the row */
        SCIP_Real oddSubsetSize
) {
    //assert(oddSubsetSize % 2 == 1);
    SCIP_CONSDATA *consdata;

    consdata = SCIPconsGetData(cons);
    assert(consdata != NULL);
    assert(consdata->row == NULL);

    SCIP_Real lhs = -SCIPinfinity(scip);
    SCIP_Real rhs = oddSubsetSize - 1;
    SCIP_CALL(SCIPcreateEmptyRowCons(scip, &consdata->row, cons, SCIPconsGetName(cons),
                                     lhs, rhs, SCIPconsIsLocal(cons), SCIPconsIsModifiable(cons),
                                     SCIPconsIsRemovable(cons)));

    SCIP_CALL(SCIPaddVarsToRow(scip, consdata->row, consdata->nvars, consdata->vars, coefs));


    return SCIP_OKAY;
}

/** does the current LP violate the logic or constraint? */
SCIP_Bool isConsLpViolated(
        SCIP *scip,               /**< SCIP data structure */
        SCIP_CONS *cons               /**< logic or constraint to be checked */
) {
    return isConsViolated(scip, cons, nullptr);
}

/** frees specific constraint data */
SCIP_RETCODE
ConshdlrOddSubsetCycles::scip_delete(SCIP *scip, SCIP_CONSHDLR */*conshdlr*/, SCIP_CONS */*cons*/,
                                     SCIP_CONSDATA **consdata) {
    assert(consdata != NULL);
    int v;

    assert(consdata != NULL);
    assert(*consdata != NULL);

    /* release the row */
    if ((*consdata)->row != nullptr) {
        SCIP_CALL(SCIPreleaseRow(scip, &(*consdata)->row));
    }

    /* release variables */
    for (v = 0; v < (*consdata)->nvars; v++) {
        assert((*consdata)->vars[v] != nullptr);
        SCIP_CALL(SCIPreleaseVar(scip, &((*consdata)->vars[v])));
    }

    SCIPfreeBlockMemoryArrayNull(scip, &(*consdata)->vars, (*consdata)->varssize);
    SCIPfreeBlockMemory(scip, consdata);

    return SCIP_OKAY;
}


SCIP_RETCODE
ConshdlrOddSubsetCycles::scip_check(SCIP * /*scip*/, SCIP_CONSHDLR * /*conshdlr*/, SCIP_CONS ** /*conss*/,
                                    int /*nconss*/,
                                    SCIP_SOL * /*sol*/,
                                    SCIP_Bool /*checkintegrality*/, SCIP_Bool /*checklprows*/,
                                    SCIP_Bool /*printreason*/, SCIP_Bool /*completely*/,
                                    SCIP_RESULT *result) {
    assert(result != nullptr);
    *result = SCIP_FEASIBLE;
    return SCIP_OKAY;
}


SCIP_RETCODE
ConshdlrOddSubsetCycles::scip_sepasol(SCIP * /*scip*/, SCIP_CONSHDLR * /*conshdlr*/, SCIP_CONS ** /*conss*/,
                                      int /*nconss*/,
                                      int /*nusefulconss*/, SCIP_SOL */*sol*/, SCIP_RESULT */*result*/) {
    throw std::runtime_error("Not implemented: scip_sepasol.");
}

SCIP_RETCODE
ConshdlrOddSubsetCycles::scip_sepalp(SCIP * /*scip*/, SCIP_CONSHDLR * /*conshdlr*/, SCIP_CONS ** /*conss*/,
                                     int /*nconss*/,
                                     int /*nusefulconss*/, SCIP_RESULT * /*result*/) {
    // 1) has no row yet -> check full constraint
    // 2) row is in lp
    // c1: not violated -> ?? check full constraint
    // c2: tight -> done
    // 3) row not in lp
    // c1: row violated -> add to lp
    // c2: not violated -> check full constraint

    throw std::runtime_error("Not implemented: scip_sepalp.");
}


/** constraint enforcing method of constraint handler for LP solutions */
SCIP_RETCODE
ConshdlrOddSubsetCycles::scip_enfolp(SCIP * /*scip*/, SCIP_CONSHDLR * /*conshdlr*/, SCIP_CONS ** /*conss*/, int nconss,
                                     int /*nusefulconss*/,
                                     unsigned int /*solinfeasible*/, SCIP_RESULT *result) {
    assert(result != nullptr);
    assert(nconss > 0);
    for (int i = 0; i < nconss; ++i) {
        //assert(conss != nullptr);
        //assert(conss[i] != nullptr);
    }

    *result = SCIP_INFEASIBLE; // Infeasible does not really mean infeasible here. See SCIP docu
    SCIP_RESULT resultSepa = SCIP_DIDNOTRUN;
    //SCIP_CALL(sepaTrianglesLp(scip, conshdlr, conss, nconss, nusefulconss, &resultSepa));
    if (resultSepa == SCIP_SEPARATED || resultSepa == SCIP_NEWROUND) *result = SCIP_SEPARATED;
    else {
        *result = SCIP_FEASIBLE;
    }

    return SCIP_OKAY;
}

/** constraint enforcing method of constraint handler for pseudo solutions */
SCIP_RETCODE
ConshdlrOddSubsetCycles::scip_enfops(SCIP * /*scip*/, SCIP_CONSHDLR * /*conshdlr*/,
                                     SCIP_CONS ** /*conss*/, int /*nconss*/, int /*nusefulconss*/,
                                     SCIP_Bool /*solinfeasible*/, SCIP_Bool /*objinfeasible*/,
                                     SCIP_RESULT *result) {
    *result = SCIP_FEASIBLE;

    throw std::runtime_error("Not implemented: scip_enfops");
}


SCIP_RETCODE
ConshdlrOddSubsetCycles::scip_lock(SCIP * /*scip*/, SCIP_CONSHDLR * /*conshdlr*/, SCIP_CONS * /*cons*/,
                                   SCIP_LOCKTYPE /*locktype*/,
                                   int /*nlockspos*/, int /*nlocksneg*/) {
    return SCIP_OKAY;
}

SCIP_RETCODE ConshdlrOddSubsetCycles::scip_trans(SCIP *scip, SCIP_CONSHDLR *conshdlr, SCIP_CONS *sourcecons,
                                                 SCIP_CONS **targetcons) {

    //SCIP_CONSDATA *sourcedata;
    SCIP_CONSDATA *targetdata = nullptr;

    // create target constraint
    SCIP_CALL(SCIPcreateCons(scip, targetcons, SCIPconsGetName(sourcecons), conshdlr, targetdata,
                             SCIPconsIsInitial(sourcecons), SCIPconsIsSeparated(sourcecons),
                             SCIPconsIsEnforced(sourcecons),
                             SCIPconsIsChecked(sourcecons), SCIPconsIsPropagated(sourcecons),
                             SCIPconsIsLocal(sourcecons),
                             SCIPconsIsModifiable(sourcecons), SCIPconsIsDynamic(sourcecons),
                             SCIPconsIsRemovable(sourcecons),
                             SCIPconsIsStickingAtNode(sourcecons)));

    return SCIP_OKAY;
}

scip::ObjProbCloneable *ConshdlrOddSubsetCycles::clone(SCIP *scip, SCIP_Bool *valid) const {
    assert(valid != nullptr);
    *valid = true;
    return new ConshdlrOddSubsetCycles(scip);
}

SCIP_RETCODE
ConshdlrOddSubsetCycles::scip_initsol(SCIP */*scip*/, SCIP_CONSHDLR */*conshdlr*/, SCIP_CONS **/*conss*/,
                                      int /*nconss*/) {
    return SCIP_OKAY;
}
