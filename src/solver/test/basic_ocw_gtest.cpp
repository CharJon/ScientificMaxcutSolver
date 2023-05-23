#include <gtest/gtest.h>

#include <filesystem>

#include "nlohmann/json.hpp"

#include "sms/io/io.hpp"
#include "sms/solver/basic_ocw.hpp"


void
benchmarkBasicSolver(const std::string &inputFile, const std::string &outputDirBase,
                     const std::string &dirPrefix,
                     int timelimit = 600) {
    mkdir(outputDirBase.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    std::cout << outputDirBase << std::endl;

    NetworKit::Graph graph = mcFileToGraph(inputFile);
    graph.indexEdges();

    // loop over seeds
    for (int i = 0; i <= 0; i++) {
        std::string outputName; //output name will be given to the solver as log dir


        outputName += outputDirBase + "/";
        outputName += dirPrefix + "_" + std::to_string(i);
        mkdir(outputName.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
        BasicOcwSolver solver(&graph, i, timelimit);
        try {
            solver.run(outputName);
            assert(solver.hasRun());
        }
        catch (std::exception &e) {
            std::cout << "File " + inputFile + " errored: " << e.what() << std::endl;
        }

    }
}

TEST(TestBasicSolver, square) {
    benchmarkBasicSolver("test/data/square.mc", "logs/square.mc", "test");
}

TEST(TestBasicSolver, pm1s_50_1) {
    benchmarkBasicSolver("test/data/pm1s_50.1.mc", "logs/pm1s_50.1.mc", "test");
}


TEST(TestBasicSolver, DISABLED_pm1s_100_3) {
    benchmarkBasicSolver("test/data/pm1s_100.3.mc", "logs/pm1s_100.3.mc", "test");
}

TEST(TestBasicSolver, DISABLED_be_120_3_5) {
    benchmarkBasicSolver("test/data/be120.3.5.mc", "logs/be120.3.5.mc", "test");
}

TEST(TestBasicSolver, DISABLED_gka4d) {
    benchmarkBasicSolver("test/data/gka4d.mc", "logs/gka4d.mc", "test");
}


