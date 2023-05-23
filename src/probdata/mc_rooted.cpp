#include "sms/probdata/mc_rooted.hpp"

void buildRootMcModel(Scip **s, const Graph *g, NetworKit::node root, bool allBinary) {
    assert(s != nullptr);

    SCIP_CALL_EXC(SCIPcreate(s))
    auto scip = *s;
    // add prob data
    auto *probdata = new ProbDataRootedMc(scip, g, root);
    SCIP_CALL_EXC(SCIPcreateObjProb(scip, "mc root triangulated model",
                                    probdata, true))
    SCIP_CALL_EXC(SCIPincludeDefaultPlugins(scip))
    SCIP_CALL_EXC(SCIPsetObjsense(scip, SCIP_OBJSENSE_MAXIMIZE))

    // Partition vars
    for (auto u: g->nodeRange()) {
        if (u != root) {
            SCIP_VAR *var;
            std::stringstream varname;
            varname << "x_" << root << "-" << u;
            auto weight = g->hasEdge(root, u) ? g->weight(root, u) : 0;
            SCIP_CALL_EXC(SCIPcreateVarBasic(scip, &var, varname.str().c_str(), 0, 1, weight, SCIP_VARTYPE_BINARY))
            SCIP_CALL_EXC(SCIPaddVar(scip, var))
            probdata->addVariable(root, u, var);
        }
    }

    // E_g vars
    for (auto e: probdata->getMcGraph()->edgeWeightRange()) {
        auto u = std::min(e.u, e.v);
        auto v = std::max(e.u, e.v);
        auto weight = e.weight;

        if (u != root && v != root) {
            SCIP_VAR *var;
            std::stringstream varname;
            varname << "x_e_" << u << "-" << v;
            SCIP_CALL_EXC(SCIPcreateVarBasic(scip, &var, varname.str().c_str(), 0, 1, weight,
                                             allBinary ? SCIP_VARTYPE_BINARY : SCIP_VARTYPE_IMPLINT))
            SCIP_CALL_EXC(SCIPaddVar(scip, var))
            probdata->addVariable(u, v, var);
        }
    }

    // Constraints
    for (auto e: g->edgeRange()) {
        if (e.u != root && e.v != root) {
            SCIP_VAR *vars[3] = {probdata->edgeToVar(e.u, e.v),
                                 probdata->nodeToVar(e.u),
                                 probdata->nodeToVar(e.v)
            };
            addXorConstraintNoCapture(scip, vars, 3);
        }
    }
}