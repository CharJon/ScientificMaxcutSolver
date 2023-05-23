#include <fstream>
#include "nlohmann/json.hpp"
#include "sms/auxiliary/mc_solution.hpp"


#define PART_0_NAME "partition_0"
#define PART_1_NAME "partition_1"

McSolution::McSolution(const Graph *const graph) : graph_(graph) {
    partition_ = std::vector<partition_t>(graph_->upperNodeIdBound(), kUNASSIGNED);
}

McSolution::McSolution(const Graph *const graph, const std::string &file) : graph_(
        graph) {
    ulong n = graph_->upperNodeIdBound();
    partition_ = std::vector<partition_t>(n, kUNASSIGNED);

    std::ifstream f(file);
    nlohmann::json data = nlohmann::json::parse(f);

    for (NetworKit::node u: data[PART_0_NAME]) {
        assert(u < n);
        assert(partition_[u] == kUNASSIGNED);
        partition_[u] = 0;
    }

    for (NetworKit::node u: data[PART_1_NAME]) {
        assert(u < n);
        assert(partition_[u] == kUNASSIGNED);
        partition_[u] = 1;
    }
}

McSolution::McSolution(const Graph *const graph, const std::vector<partition_t> & /*partitions*/) : graph_(
        graph) {
    throw std::runtime_error("Not implemented!");
}

McSolution::McSolution(const Graph *const graph, const std::vector<Edge> & /*cutEdges*/) : graph_(graph) {
    throw std::runtime_error("Not implemented!");
}

std::vector<Edge> McSolution::getCutEdges() const {
    std::vector<Edge> cutEdges{};
    for (auto e: graph_->edgeWeightRange()) {
        partition_t partU = partition_[e.u];
        partition_t partV = partition_[e.v];
        assert(partU <= 1);
        assert(partV <= 1);
        if (partU ^ partV) cutEdges.push_back(e);
    }
    return cutEdges;
}

edgeweight McSolution::getCutValue() const {
    edgeweight cutValue = 0;
    for (auto e: graph_->edgeWeightRange()) {
        partition_t partU = partition_[e.u];
        partition_t partV = partition_[e.v];
        assert(partU <= 1);
        assert(partV <= 1);
        cutValue += (partU ^ partV) * e.weight;
    }
    return cutValue;
}

void McSolution::flipPartitioning() {
    for (auto u: graph_->nodeRange()) {
        partition_[u] = partition_[u] == kUNASSIGNED ? kUNASSIGNED : 1 - partition_[u];
    }
}

void McSolution::toPartition0(node u) {
    assert(graph_->hasNode(u));
    partition_[u] = 0;
}

void McSolution::toPartition1(node u) {
    assert(graph_->hasNode(u));
    partition_[u] = 1;
}

void McSolution::allNodesTo0() {
    for (auto u: graph_->nodeRange()) {
        toPartition0(u);
    }
}

void McSolution::allNodesTo1() {
    for (auto u: graph_->nodeRange()) {
        toPartition1(u);
    }
}

std::vector<node> McSolution::getPartition0() const {
    std::vector<node> part0nodes{};
    for (auto u: graph_->nodeRange()) {
        if (partition_[u] == 0) part0nodes.push_back(u);
    }
    return part0nodes;
}

std::vector<node> McSolution::getPartition1() const {
    std::vector<node> part1nodes{};
    for (auto u: graph_->nodeRange()) {
        if (partition_[u] == 1) part1nodes.push_back(u);
    }
    return part1nodes;
}

void McSolution::saveToFile(const std::string &path) const {
    nlohmann::json save;

    save[PART_0_NAME] = getPartition0();
    save[PART_1_NAME] = getPartition1();

    std::ofstream file(path);
    file << std::setw(4) << save << std::endl;
}


bool McSolution::isValid() const {
    return std::all_of(partition_.begin(), partition_.end(), [&](partition_t u) { return u != kUNASSIGNED; });
}

uint8_t McSolution::getPartition(node u) const {
    assert(graph_->hasNode(u));
    return partition_[u];
}
