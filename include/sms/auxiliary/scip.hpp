#ifndef SMS_SCIP_HPP
#define SMS_SCIP_HPP

#include <string>

#include <scip/scip.h>
#include "nlohmann/json.hpp"

void scipFinalStatsToJsonFile(SCIP *scip, const std::string &output_file_name, nlohmann::json j = nlohmann::json());

bool scipVarIsFixedLocal(SCIP *scip, SCIP_Var *var);

void turnOffAllCuts(SCIP *scip);

void turnOffAllPrimalHeuristics(SCIP *scip);

void turnOffConflictAnalysis(SCIP *scip);

SCIP_Real normalizedFractionality(SCIP_Real);

bool consArrayIsValid(int nconss, SCIP_CONS **conss);

bool isBinary(Scip *scip, SCIP_Var *var);

bool isBinary(SCIP *scip, SCIP_VAR *var, SCIP_Sol *sol);

void printScipStatistics(SCIP *scip, const std::string &outputFileName);

bool isFreeLocal(SCIP_VAR *var);

int numberOfLpThreads(SCIP *scip);

void
addConstraintNoCapture(SCIP *scip, SCIP_Real coeffs[], SCIP_VAR *vars[], size_t nvars, SCIP_Real lhs, SCIP_Real rhs);

void
addXorConstraintNoCapture(SCIP *scip, SCIP_VAR *vars[], size_t nvars);

void addLogicOrConstraintNoCapture(SCIP *scip, SCIP_VAR *vars[], size_t nvars);

#endif //SMS_SCIP_HPP
