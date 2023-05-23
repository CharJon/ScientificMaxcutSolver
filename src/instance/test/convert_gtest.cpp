#include <gtest/gtest.h>
#include "sms/instance/convert.hpp"

#include "networkit/io/EdgeListReader.hpp"

#include "sms/instance/qubo.hpp"

NetworKit::Graph readGraph(const std::string &path) {
    auto er = NetworKit::EdgeListReader(' ', 0, "#");
    auto g = er.read(path);
    assert(g.isWeighted());
    return g;
}

TEST(MaxCutToQuBO, BasicTest) {
    NetworKit::Graph g = readGraph("test/data/square.wel");
    const int dim = 4;

    MaxCut mc(g);
    auto qubo = maxCutToQUBO(mc);

    double solutionVector[dim] = {1, 0, 0, 1};
    ASSERT_EQ(qubo.getSolutionValue(solutionVector), -1.1);

    solutionVector[0] = 0;
    solutionVector[1] = 1;
    solutionVector[2] = 1;
    solutionVector[3] = 0;
    ASSERT_EQ(qubo.getSolutionValue(solutionVector), -1.1);
}

TEST(MaxCutToQuBO, MediumTest) {
    NetworKit::Graph g = readGraph("test/data/tri_square_tri.wel");
    const int dim = 6;

    MaxCut mc(g);
    auto qubo = maxCutToQUBO(mc);

    double solutionVector[dim] = {1, 1, 0, 1, 1, 0};
    ASSERT_EQ(qubo.getSolutionValue(solutionVector), -11.5);
}
