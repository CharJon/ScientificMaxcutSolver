#ifndef SMS_DIMACS_READER_HPP
#define SMS_DIMACS_READER_HPP

#include <string>

#include "networkit/graph/Graph.hpp"

#include "sms/io/io.hpp"
#include "sms/instance/ising.hpp"

class DimacsReader {
public:
    explicit DimacsReader(const std::string &path, double scaling = 1.0, bool isMinimize = false);

    Ising getInstance();

    NetworKit::Graph getGraph();

private:
    NetworKit::Graph graph_;
    double scaling_;
};


#endif //SMS_DIMACS_READER_HPP
