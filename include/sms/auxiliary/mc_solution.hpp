#ifndef SMS_MCSOLUTION_HPP
#define SMS_MCSOLUTION_HPP


#include <memory>
#include <fstream>
#include "nlohmann/json.hpp"
#include "networkit/graph/Graph.hpp"

using NetworKit::Graph;
using NetworKit::Edge;
using NetworKit::node;
using NetworKit::edgeweight;
using partition_t = uint8_t;

class McSolution {
public:
    explicit McSolution(const Graph *graph);

    explicit McSolution(const Graph *graph, const std::string &file);

    // not implemented yet
    explicit McSolution(const Graph *graph, const std::vector<partition_t> &partitions);

    // not implemented yet
    explicit McSolution(const Graph *graph, const std::vector<Edge> &cutEdges);

    void toPartition0(node u);

    void toPartition1(node u);

    void allNodesTo0();

    void allNodesTo1();

    std::vector<Edge> getCutEdges() const;

    edgeweight getCutValue() const;

    std::vector<node> getPartition0() const;

    std::vector<node> getPartition1() const;

    uint8_t getPartition(node u) const;

    void flipPartitioning();

    void saveToFile(const std::string &) const;

    bool isValid() const;

    const uint8_t kUNASSIGNED = 2; // used for unassigned nodes

private:
    const Graph *const graph_;
    std::vector<partition_t> partition_;

};

McSolution readMcSolutionFromFile(const std::string &path);


#endif //SMS_MCSOLUTION_HPP
