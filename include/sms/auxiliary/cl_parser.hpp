#ifndef SMS_CL_PARSER_HPP
#define SMS_CL_PARSER_HPP

#include <optional>

#include "cxxopts.hpp"

class CLParser {
public:
    CLParser(int argc, char **argv);

    [[nodiscard]] std::optional<std::string> getFileName() const;

    [[nodiscard]] std::string getType() const;

    [[nodiscard]] int getSeed() const;

    [[nodiscard]] double getTimeLimit() const;

    [[nodiscard]] int getNodeLimit() const;

    [[nodiscard]] std::string getParamFile() const;

    [[nodiscard]] std::optional<std::string> getStatsFile() const;

    [[nodiscard]] std::optional<std::string> getStatsScipFile() const;

    [[nodiscard]] std::optional<std::string> getLogScipFile() const;

    [[nodiscard]] std::optional<std::string> getHistoryFile() const;

    [[nodiscard]] std::optional<std::string> getSolutionFile() const;

    [[nodiscard]] std::optional<std::string> getWarmStartSolutionFile() const;

    [[nodiscard]] bool getNodeVars() const;

private:
    cxxopts::ParseResult parseResult_;

    static cxxopts::Options createParser();

};

#endif //SMS_CL_PARSER_HPP
