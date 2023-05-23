#include <cstdlib>
#include <iostream>

#include "networkit/graph/Graph.hpp"
#include "networkit/io/EdgeListReader.hpp"
#include "networkit/auxiliary/Parallelism.hpp"

#include "sms/io/io.hpp"
#include "sms/auxiliary/cl_parser.hpp"
#include "sms/auxiliary/graphs.hpp"
#include "sms/solver/basic_ocw.hpp"


/** main function for sms example */
int
main(
        int args,
        char **argv
) {
    std::cout << args << " |";

    for (int i = 0; i < args; i++) {
        std::cout << " " << argv[i] << " |";
    }
    std::cout << std::endl;

    Aux::setNumberOfThreads(1);

    auto clArgs = CLParser(args, argv);

    std::cout << "********************************************" << std::endl;
    std::cout << "* max cut solver based on SCIP            *" << std::endl;
    std::cout << "********************************************" << std::endl << std::endl;


    NetworKit::Graph graph;
    if (!clArgs.getFileName().has_value()) {
        std::cout << "No graph , using test graph. Else" << std::endl;
        std::cout << "call " << argv[0] << " <path/to/graph>" << std::endl;

        graph = test_graph();
    } else {
        std::string graph_path = clArgs.getFileName().value();
        std::string input_type = clArgs.getType();

        std::cout << graph_path << " is <path/to/graph>" << std::endl;
        std::cout << input_type << " is <type>" << std::endl;

        if (input_type == "mc") {
            graph = mcFileToGraph(graph_path);
            graph.removeNode(0);
            auto r = compactGraph(graph);
            graph = std::move(r.compact_graph);
        } else if (input_type == "wel") {
            NetworKit::EdgeListReader el(' ', 1);
            graph = el.read(graph_path);
        } else if (input_type == "bq") {
            graph = bqFileToGraph(graph_path);
        } else {
            std::cerr << "Type " << input_type << " is unknown" << std::endl;
            return EXIT_FAILURE;
        }
    }
    graph.indexEdges();

    printGraphInformation(graph);

    double timelimit = clArgs.getTimeLimit();
    int nodelimit = clArgs.getNodeLimit();
    int seed = clArgs.getSeed();
    const std::string parameterFile = clArgs.getParamFile();
    const std::string statsFile = clArgs.getStatsFile().has_value() ? clArgs.getStatsFile().value() : "";
    const std::string statsScipFile = clArgs.getStatsScipFile().has_value() ? clArgs.getStatsScipFile().value() : "";
    const std::string logScipFile = clArgs.getLogScipFile().has_value() ? clArgs.getLogScipFile().value() : "";
    const std::string historyFile = clArgs.getHistoryFile().has_value() ? clArgs.getHistoryFile().value() : "";

    BasicOcwSolver solver(&graph, seed, timelimit, nodelimit, parameterFile);

    if (clArgs.getWarmStartSolutionFile().has_value()) {
        std::string warmStartSolutionFile = clArgs.getWarmStartSolutionFile().value();
        auto warmStartSolution = McSolution(&graph, warmStartSolutionFile);
        solver.addWarmStartSolution(warmStartSolution);
    }

    auto bestSolution = solver.run(statsFile, statsScipFile, logScipFile, historyFile, clArgs.getNodeVars());

    if (clArgs.getSolutionFile().has_value()) {
        std::string solutionFile = clArgs.getSolutionFile().value();
        bestSolution.saveToFile(solutionFile);
    }


    return EXIT_SUCCESS;
}
