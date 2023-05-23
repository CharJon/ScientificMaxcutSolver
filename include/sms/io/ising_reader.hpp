#ifndef SMS_SG_PARSER_HPP
#define SMS_SG_PARSER_HPP

#include <string>

#include "sms/io/io.hpp"
#include "sms/instance/ising.hpp"

class sgParser {
public:
    sgParser(const std::string &path, double scaling = 1.0, double offset = 0.0);

    Ising getInstance();

private:
    NetworKit::Graph graph_;
    double scaling_;
    double offset_;
};

class gsgParser {
public:
    gsgParser(const std::string &path, double scaling = 1.0, double offset = 0.0);

    Ising getInstance();

private:
    NetworKit::Graph graph_;
    double scaling_;
    double offset_;
    int dim_;
    int gridLength_;

    void isValid();
};

#endif //SMS_SG_PARSER_HPP
