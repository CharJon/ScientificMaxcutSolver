#include <optional>

#include "cxxopts.hpp"

#include "sms/auxiliary/cl_parser.hpp"

#define INSTANCE "instance"
#define TYPE "type"
#define SEED "seed"
#define TIMELIMIT "timelimit"
#define NODELIMIT "nodelimit"
#define PARAMFILE "paramfile"
#define STATSFILE "statsfile"
#define STATSSCIPFILE "stats-scip"
#define LOGSCIPFILE "log-scip"
#define HISTORYFILE "history"
#define SOLUTIONFILE "solution"
#define WARMSTARTSOLUTIONFILE "warm-start-solution"
#define NODEVARS "node-vars"

cxxopts::Options CLParser::createParser() {
    cxxopts::Options options("SMS", "Max cut solver based on SCIP  ");

    options.add_options()
            // positional
            (INSTANCE, "Instance file name", cxxopts::value<std::string>())
            (TYPE, "Type (optional)", cxxopts::value<std::string>())
            // pseudo-optional (have default values)
            (SEED, "Random seed", cxxopts::value<int>()->default_value("0"))
            (TIMELIMIT, "Maximum time in seconds", cxxopts::value<double>()->default_value("1e20"))
            (NODELIMIT, "Maximum number of nodes", cxxopts::value<int>()->default_value("-1"))
            (PARAMFILE, "Parameter setting file to use",
             cxxopts::value<std::string>()->default_value("params/default_params_dual.set"))
            (NODEVARS, "Model with node vars", cxxopts::value<bool>()->default_value("true"))
            // real optional
            (STATSFILE, "File to store compact stats in", cxxopts::value<std::string>())
            (STATSSCIPFILE, "", cxxopts::value<std::string>())
            (LOGSCIPFILE, "", cxxopts::value<std::string>())
            (HISTORYFILE, "", cxxopts::value<std::string>())
            (SOLUTIONFILE, "Solution file", cxxopts::value<std::string>())
            (WARMSTARTSOLUTIONFILE, "Solution file to use as a warm-start", cxxopts::value<std::string>());
    options.parse_positional({INSTANCE, TYPE});

    return options;
}


CLParser::CLParser(int argc, char **argv) {
    parseResult_ = createParser().parse(argc, argv);
}

std::optional<std::string> CLParser::getFileName() const {
    if (parseResult_.count(INSTANCE) == 0)
        return {};
    else
        return parseResult_[INSTANCE].as<std::string>();
}

std::string CLParser::getType() const {
    if (parseResult_.count(TYPE) == 0) {
        unsigned long positionLastDot = getFileName().value().find_last_of(".");
        return getFileName().value().substr(positionLastDot + 1);
    } else {
        return parseResult_[TYPE].as<std::string>();
    }
}

int CLParser::getSeed() const {
    return parseResult_[SEED].as<int>();
}

double CLParser::getTimeLimit() const {
    return parseResult_[TIMELIMIT].as<double>();
}


std::optional<std::string> CLParser::getWarmStartSolutionFile() const {
    if (parseResult_.count(WARMSTARTSOLUTIONFILE) == 0)
        return std::nullopt;
    else
        return parseResult_[WARMSTARTSOLUTIONFILE].as<std::string>();
}

std::optional<std::string> CLParser::getSolutionFile() const {
    if (parseResult_.count(SOLUTIONFILE) == 0)
        return std::nullopt;
    else
        return parseResult_[SOLUTIONFILE].as<std::string>();
}

std::optional<std::string> CLParser::getHistoryFile() const {
    if (parseResult_.count(HISTORYFILE) == 0)
        return std::nullopt;
    else
        return parseResult_[HISTORYFILE].as<std::string>();
}

std::optional<std::string> CLParser::getLogScipFile() const {
    if (parseResult_.count(LOGSCIPFILE) == 0)
        return std::nullopt;
    else
        return parseResult_[LOGSCIPFILE].as<std::string>();
}

std::optional<std::string> CLParser::getStatsScipFile() const {
    if (parseResult_.count(STATSSCIPFILE) == 0)
        return std::nullopt;
    else
        return parseResult_[STATSSCIPFILE].as<std::string>();
}

std::optional<std::string> CLParser::getStatsFile() const {
    if (parseResult_.count(STATSFILE) == 0)
        return std::nullopt;
    else
        return parseResult_[STATSFILE].as<std::string>();
}

std::string CLParser::getParamFile() const {
    return parseResult_[PARAMFILE].as<std::string>();
}

int CLParser::getNodeLimit() const {
    return parseResult_[NODELIMIT].as<int>();
}

bool CLParser::getNodeVars() const {
    return parseResult_[NODEVARS].as<bool>();
}

