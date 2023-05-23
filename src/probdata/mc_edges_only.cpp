#include "sms/probdata/mc_edges_only.hpp"

void buildEdgesOnlyMcModel(Scip **s, const Graph *g) {
    assert(s != nullptr);

    SCIP_CALL_EXC(SCIPcreate(s))
    auto scip = *s;
    // add prob data
    auto *probdata = new ProbDataEdgesOnlyMc(scip, g);
    SCIP_CALL_EXC(SCIPcreateObjProb(scip, "mc edge-model",
                                    probdata, true))
    SCIP_CALL_EXC(SCIPincludeDefaultPlugins(scip))
    SCIP_CALL_EXC(SCIPsetObjsense(scip, SCIP_OBJSENSE_MAXIMIZE))

    // vars
    for (auto e: probdata->getMcGraph()->edgeWeightRange()) {
        auto u = std::min(e.u, e.v);
        auto v = std::max(e.u, e.v);
        auto weight = e.weight;

        SCIP_VAR *var;
        std::stringstream varname;
        varname << "x_e_" << u << "-" << v;
        SCIP_CALL_EXC(SCIPcreateVarBasic(scip, &var, varname.str().c_str(), 0, 1, weight, SCIP_VARTYPE_BINARY))
        SCIP_CALL_EXC(SCIPaddVar(scip, var))
        probdata->addVariable(u, v, var);
    }
}
