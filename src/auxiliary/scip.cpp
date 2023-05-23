#include <string>
#include <fstream>
#include "nlohmann/json.hpp"
#include "scip/scip.h"
#include "scip/cons_linear.h"
#include "scip/cons_xor.h"
#include "scip/cons_logicor.h"
#include "sms/auxiliary/scip_exception.hpp"
#include "sms/auxiliary/scip.hpp"


void
addConstraintNoCapture(SCIP *scip, SCIP_Real coeffs[], SCIP_VAR *vars[], size_t nvars, SCIP_Real lhs, SCIP_Real rhs) {
    SCIP_CONS *cons;
    SCIP_CALL_EXC(SCIPcreateConsBasicLinear(scip, &cons, "", nvars, vars, coeffs, lhs, rhs))
    SCIP_CALL_EXC(SCIPaddCons(scip, cons))
    SCIP_CALL_EXC(SCIPreleaseCons(scip, &cons))
}

void addLogicOrConstraintNoCapture(SCIP *scip, SCIP_VAR *vars[], size_t nvars) {
    SCIP_CONS *cons;
    SCIP_CALL_EXC(SCIPcreateConsBasicLogicor(scip, &cons, "", nvars, vars))
    SCIP_CALL_EXC(SCIPaddCons(scip, cons))
    SCIP_CALL_EXC(SCIPreleaseCons(scip, &cons))
}

void addXorConstraintNoCapture(SCIP *scip, SCIP_VAR *vars[], size_t nvars) {
    SCIP_CONS *cons;
    SCIP_CALL_EXC(
            SCIPcreateConsXor(scip, &cons, "", FALSE, nvars, vars,
                              TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE))
    SCIP_CALL_EXC(SCIPaddCons(scip, cons))
    SCIP_CALL_EXC(SCIPreleaseCons(scip, &cons))
}


void scipFinalStatsToJsonFile(SCIP *scip, const std::string &output_file_name, nlohmann::json j) {
    j["cpu_solving_time"] = SCIPgetSolvingTime(scip);
    j["best_solution_value"] = SCIPgetSolOrigObj(scip, SCIPgetBestSol(scip));
    j["optimal_solution_found"] = SCIPgetStatus(scip) == SCIP_STATUS_OPTIMAL;
    j["timelimit_reached"] = SCIPgetStatus(scip) == SCIP_STATUS_TIMELIMIT;
    j["bab_nodes"] = SCIPgetNTotalNodes(scip);
    j["final_bound_value"] = SCIPgetDualbound(scip);
    // ToDo (JC): There is currently no way to get this information
    j["root_bound_value"] = nullptr;
    int seed = 0;
    SCIPgetIntParam(scip, "randomization/randomseedshift", &seed);
    j["scip_randomseedshift"] = seed;
    double timelimit = 0;
    SCIPgetRealParam(scip, "limits/time", &timelimit);
    j["scip_timelimit"] = timelimit;

    std::ofstream o(output_file_name);
    o << std::setw(4) << j << std::endl;
}

inline bool scipVarIsFixedLocal(SCIP *scip, SCIP_Var *var) {
    return SCIPisEQ(scip, SCIPvarGetLbLocal(var), SCIPvarGetUbLocal(var));
}

/*
 * Fractionality between zero (not fractional) and one (fractional part is 0.5).
 */
inline SCIP_Real normalizedFractionality(SCIP_Real val) {
    return 1 - 2 * abs(0.5 - val);
}


bool consArrayIsValid(int nconss, SCIP_CONS **conss) {
    bool isValid = true;
    for (int i = 0; i < nconss; ++i) {
        isValid &= (conss != nullptr);
        isValid &= (conss[i] != nullptr);
    }
    return isValid;
}

bool isFreeLocal(SCIP_VAR *var) {
    return (SCIPvarGetUbLocal(var) - SCIPvarGetLbLocal(var)) > 0.5;
}

void printScipStatistics(SCIP *scip, const std::string &outputFileName) {
    auto scipStatisticsFile = fopen(outputFileName.c_str(), "w");
    SCIPprintStatistics(scip, scipStatisticsFile);
    fclose(scipStatisticsFile);
}

bool isBinary(SCIP *scip, SCIP_VAR *var, SCIP_Sol *sol) {
    auto lpVal = SCIPgetSolVal(scip, sol, var);
    bool isZero = SCIPisEQ(scip, lpVal, 0);
    bool isOne = SCIPisEQ(scip, lpVal, 1);
    return isZero || isOne;
}

bool isBinary(SCIP *scip, SCIP_VAR *var) {
    auto lpVal = SCIPgetSolVal(scip, nullptr, var);
    bool isZero = SCIPisEQ(scip, lpVal, 0);
    bool isOne = SCIPisEQ(scip, lpVal, 1);
    return isZero || isOne;
}

int numberOfLpThreads(SCIP *scip) {
    int nThreads = -1;
    SCIP_LPI *lpi = nullptr;
    SCIP_CALL_EXC(SCIPgetLPI(scip, &lpi))
    if (lpi != nullptr) {
        SCIP_CALL_EXC(SCIPlpiGetIntpar(lpi, SCIP_LPPAR_THREADS, &nThreads))
    }
    return nThreads;
}


void turnOffAllCuts(SCIP *scip) {
    SCIPsetIntParam(scip, "separating/cgmip/freq", -1);
    SCIPsetIntParam(scip, "separating/clique/freq", -1);
    SCIPsetIntParam(scip, "separating/closecuts/freq", -1);
    SCIPsetIntParam(scip, "separating/flowcover/freq", -1);
    SCIPsetIntParam(scip, "separating/cmir/freq", -1);
    SCIPsetIntParam(scip, "separating/knapsackcover/freq", -1);
    SCIPsetIntParam(scip, "separating/aggregation/freq", -1);
    SCIPsetIntParam(scip, "separating/convexproj/freq", -1);
    SCIPsetIntParam(scip, "separating/disjunctive/freq", -1);
    SCIPsetIntParam(scip, "separating/eccuts/freq", -1);
    SCIPsetIntParam(scip, "separating/gauge/freq", -1);
    SCIPsetIntParam(scip, "separating/gomory/freq", -1);
    SCIPsetIntParam(scip, "separating/strongcg/freq", -1);
    SCIPsetIntParam(scip, "separating/gomorymi/freq", -1);
    SCIPsetIntParam(scip, "separating/impliedbounds/freq", -1);
    SCIPsetIntParam(scip, "separating/interminor/freq", -1);
    SCIPsetIntParam(scip, "separating/intobj/freq", -1);
    SCIPsetIntParam(scip, "separating/mcf/freq", -1);
    SCIPsetIntParam(scip, "separating/minor/freq", -1);
    SCIPsetIntParam(scip, "separating/oddcycle/freq", -1);
    SCIPsetIntParam(scip, "separating/rapidlearning/freq", -1);
    SCIPsetIntParam(scip, "separating/zerohalf/freq", -1);
}

void turnOffConflictAnalysis(SCIP *scip) {
    SCIPsetBoolParam(scip, "conflict/enable", 0);
}

void turnOffAllPrimalHeuristics(SCIP *scip) {
    SCIPsetIntParam(scip, "heuristics/actconsdiving/freq", -1);
    SCIPsetIntParam(scip, "heuristics/adaptivediving/freq", -1);
    SCIPsetIntParam(scip, "heuristics/bound/freq", -1);
    SCIPsetIntParam(scip, "heuristics/clique/freq", -1);
    SCIPsetIntParam(scip, "heuristics/coefdiving/freq", -1);
    SCIPsetIntParam(scip, "heuristics/completesol/freq", -1);
    SCIPsetIntParam(scip, "heuristics/conflictdiving/freq", -1);
    SCIPsetIntParam(scip, "heuristics/crossover/freq", -1);
    SCIPsetIntParam(scip, "heuristics/dins/freq", -1);
    SCIPsetIntParam(scip, "heuristics/distributiondiving/freq", -1);
    SCIPsetIntParam(scip, "heuristics/dps/freq", -1);
    SCIPsetIntParam(scip, "heuristics/dualval/freq", -1);
    SCIPsetIntParam(scip, "heuristics/farkasdiving/freq", -1);
    SCIPsetIntParam(scip, "heuristics/feaspump/freq", -1);
    SCIPsetIntParam(scip, "heuristics/fixandinfer/freq", -1);
    SCIPsetIntParam(scip, "heuristics/fracdiving/freq", -1);
    SCIPsetIntParam(scip, "heuristics/gins/freq", -1);
    SCIPsetIntParam(scip, "heuristics/guideddiving/freq", -1);
    SCIPsetIntParam(scip, "heuristics/zeroobj/freq", -1);
    SCIPsetIntParam(scip, "heuristics/indicator/freq", -1);
    SCIPsetIntParam(scip, "heuristics/intdiving/freq", -1);
    SCIPsetIntParam(scip, "heuristics/intshifting/freq", -1);
    SCIPsetIntParam(scip, "heuristics/linesearchdiving/freq", -1);
    SCIPsetIntParam(scip, "heuristics/localbranching/freq", -1);
    SCIPsetIntParam(scip, "heuristics/locks/freq", -1);
    SCIPsetIntParam(scip, "heuristics/lpface/freq", -1);
    SCIPsetIntParam(scip, "heuristics/alns/freq", -1);
    SCIPsetIntParam(scip, "heuristics/nlpdiving/freq", -1);
    SCIPsetIntParam(scip, "heuristics/mutation/freq", -1);
    SCIPsetIntParam(scip, "heuristics/multistart/freq", -1);
    SCIPsetIntParam(scip, "heuristics/mpec/freq", -1);
    SCIPsetIntParam(scip, "heuristics/objpscostdiving/freq", -1);
    SCIPsetIntParam(scip, "heuristics/octane/freq", -1);
    SCIPsetIntParam(scip, "heuristics/ofins/freq", -1);
    SCIPsetIntParam(scip, "heuristics/oneopt/freq", -1);
    SCIPsetIntParam(scip, "heuristics/padm/freq", -1);
    SCIPsetIntParam(scip, "heuristics/proximity/freq", -1);
    SCIPsetIntParam(scip, "heuristics/pscostdiving/freq", -1);
    SCIPsetIntParam(scip, "heuristics/randrounding/freq", -1);
    SCIPsetIntParam(scip, "heuristics/rens/freq", -1);
    SCIPsetIntParam(scip, "heuristics/reoptsols/freq", -1);
    SCIPsetIntParam(scip, "heuristics/repair/freq", -1);
    SCIPsetIntParam(scip, "heuristics/rins/freq", -1);
    SCIPsetIntParam(scip, "heuristics/rootsoldiving/freq", -1);
    SCIPsetIntParam(scip, "heuristics/rounding/freq", -1);
    SCIPsetIntParam(scip, "heuristics/shiftandpropagate/freq", -1);
    SCIPsetIntParam(scip, "heuristics/shifting/freq", -1);
    SCIPsetIntParam(scip, "heuristics/simplerounding/freq", -1);
    SCIPsetIntParam(scip, "heuristics/subnlp/freq", -1);
    SCIPsetIntParam(scip, "heuristics/trivial/freq", -1);
    SCIPsetIntParam(scip, "heuristics/trivialnegation/freq", -1);
    SCIPsetIntParam(scip, "heuristics/trustregion/freq", -1);
    SCIPsetIntParam(scip, "heuristics/trysol/freq", -1);
    SCIPsetIntParam(scip, "heuristics/twoopt/freq", -1);
    SCIPsetIntParam(scip, "heuristics/undercover/freq", -1);
    SCIPsetIntParam(scip, "heuristics/vbounds/freq", -1);
    SCIPsetIntParam(scip, "heuristics/veclendiving/freq", -1);
    SCIPsetIntParam(scip, "heuristics/zirounding/freq", -1);
}
