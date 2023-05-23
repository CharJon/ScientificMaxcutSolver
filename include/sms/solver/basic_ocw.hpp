#ifndef SMS_BASIC_OCW_HPP
#define SMS_BASIC_OCW_HPP

#include "networkit/graph/Graph.hpp"

#include "scip/scip.h"
#include "scip/scipdefplugins.h"

#include "sms/auxiliary/scip_exception.hpp"
#include "sms/auxiliary/scip.hpp"
#include "sms/auxiliary/mc_solution.hpp"

using graph_ptr = NetworKit::Graph const *const;


class BasicOcwSolver {
public:
    BasicOcwSolver(graph_ptr g, int seed, double timelimit, int nodelimit = -1);

    BasicOcwSolver(graph_ptr g, int seed, double timelimit, int nodelimit, std::string paramFile);

    ~BasicOcwSolver();

    bool hasRun() const;

    McSolution run(const std::string &stats = "", const std::string &statsScip = "", const std::string &logScip = "",
                   const std::string &history = "",
                   bool nodeVars = false);

    void addWarmStartSolution(const McSolution &solution);

private:
    SCIP *scip_;
    graph_ptr g_;
    bool run_ = false;
    int seed_;
    double timelimit_;
    int nodelimit_;
    std::string paramFile_;

    std::vector<McSolution> warmStartSolutions_;

    void setScipParams();

    void addWarmStartSolutionsToScip();

    void addWarmStartSolutionsEdgesOnlyToScip();

    McSolution getBestSolution();

    McSolution getBestSolutionEdgesOnly();

    McSolution solveRooted(const std::string &stats, const std::string &statsScip, const std::string &logScip,
                           const std::string &history);

    McSolution solveEdgesOnly(const std::string &stats, const std::string &statsScip, const std::string &logScip,
                              const std::string &history);
};

#endif //SMS_BASIC_OCW_HPP
