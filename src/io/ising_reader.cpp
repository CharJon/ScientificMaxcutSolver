#include "sms/io/ising_reader.hpp"


sgParser::sgParser(const std::string &path, double scaling, double offset) {
    scaling_ = scaling;
    offset_ = offset;

    graph_ = mcFileToGraph(path);
    graph_.removeNode(0); // node 0 is added but not used
}

Ising sgParser::getInstance() {
    return Ising(graph_, scaling_, offset_);
}

gsgParser::gsgParser(const std::string &path, double scaling, double offset) {
    scaling_ = scaling;
    offset_ = offset;
    graph_ = mcFileToGraph(path);
    graph_.removeNode(0); // node 0 is added but not used

    isValid();
}

Ising gsgParser::getInstance() {
    return Ising(graph_, scaling_, offset_);
}

void gsgParser::isValid() {
    auto n = static_cast<double>(graph_.numberOfNodes());
    auto m = graph_.numberOfEdges();
    double possibleGridLength = 0.0;
    dim_ = -1;

    if (m % 2 == 0) {
        possibleGridLength = pow(n, 1.0 / 2.0);
        if ((possibleGridLength - round(possibleGridLength) <=
             std::numeric_limits<double>::epsilon())) {
            dim_ = 2;
            gridLength_ = static_cast<int>(round(possibleGridLength)); // round to use later
        }
    }
    if (m % 3 == 0) {
        possibleGridLength = pow(n, 1.0 / 3.0);
        if ((possibleGridLength - round(possibleGridLength) <=
             std::numeric_limits<double>::epsilon())) {
            dim_ = 3;
            gridLength_ = static_cast<int>(round(possibleGridLength)); // round to use later
        }
    }

    if (dim_ == -1) {// if no matching dim has been found -> error
        throw std::runtime_error("Grid sized could not be determined for given number of spins and interactions");
    }

    if (n * dim_ != m)
        throw std::runtime_error(
                "Number of interactions does not match expected value: Expected " + std::to_string(n * dim_) + " got " +
                std::to_string(m));

    for (auto i: graph_.nodeRange()) {
        for (int d = 0; d < dim_; d++) {
            node gTod = static_cast<node>(pow(gridLength_, d));
            node gTodplusOne = static_cast<node>(pow(gridLength_, d + 1));
            node u = i - 1;
            node requested = u + gTod;
            ulong depth = u / gTodplusOne + 1;
            node v = requested - ((requested) >= gTodplusOne * depth ? gTodplusOne : 0);

            if (!graph_.hasEdge(u + 1, v + 1)) {
                throw std::runtime_error(
                        "Interaction between " + std::to_string(u + 1) + " and " + std::to_string(v + 1) +
                        " not specified");
            }

        }
    }
}
