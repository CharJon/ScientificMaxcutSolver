#include <iostream>
#include <regex>
#include <tuple>
#include <vector>

#include "sms/io/io.hpp"

NetworKit::Graph test_graph() {
    auto graph = NetworKit::Graph(6, true);
    graph.addEdge(1, 2, 5);
    graph.addEdge(2, 3, 5);
    graph.addEdge(1, 4, -2);
    graph.addEdge(2, 5, -7);
    graph.addEdge(3, 5, -3);
    graph.addEdge(4, 5, 9);
    return graph;
}

bool sortByBoth(const std::tuple<int, int, double> &a, const std::tuple<int, int, double> &b) {
    if (std::get<0>(a) == std::get<0>(b)) {
        return (std::get<1>(a) < std::get<1>(b));
    } else {
        return (std::get<0>(a) < std::get<0>(b));
    }
}

std::tuple<unsigned int, unsigned int, std::vector<std::string>> parseHeader(std::vector<std::string> file_vector) {
    std::vector<std::string> header = splitString(file_vector[0], ' ');
    unsigned int edges;
    unsigned int nodes;

    if (header.size() != 2) {
        throw std::runtime_error(
                "Header information incorrect. " + std::to_string(header.size()) + " elements provided, 2 expected.");
    } else {
        nodes = stringToInt(header[0]);
        edges = stringToInt(header[1]);
    }


    file_vector.erase(file_vector.begin());

    return std::make_tuple(nodes, edges, file_vector);
}

std::vector<std::string> fileToVector(const std::string &path) {
    std::ifstream file(path);
    std::vector<std::string> vec = std::vector<std::string>();
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            vec.push_back(line);
        }
    } else {
        throw std::runtime_error("File could not be opened: " + path + ".");
    }
    file.close();

    return vec;
}

std::tuple<std::vector<std::string>, std::vector<std::string>>
parseComments(const std::vector<std::string> &file_vector) {
    std::vector<std::string> comments = std::vector<std::string>();
    std::vector<std::string> remainder = std::vector<std::string>();

    bool comment_block = true;

    for (auto line: file_vector) {
        if ((line[0] == '#') && comment_block)  //Allow comments only inside the comment block
        {
            comments.push_back(line);
        } else if ((line[0] == '#') && !comment_block)    //Error if comments are not inside the comment block
        {
            throw std::logic_error("Comment after beginning of file.");
        } else {
            remainder.push_back(line);
            comment_block = false;  //End the comment block after first non comment line
        }
    }

    return std::make_tuple(comments, remainder);
}

std::vector<std::tuple<int, int, double>>
parseEdgelist(const std::vector<std::string> &edgelist, int lower, int upper) {
    std::vector<std::tuple<int, int, double>> res = std::vector<std::tuple<int, int, double>>();


    for (const auto &edge: edgelist) {
        auto [a, b, w] = parseEdge(edge);

        if ((a < lower) || (a > upper) || (b < lower) || (b > upper)) {
            throw std::runtime_error(
                    "Node Id " + std::to_string(a) + " or " + std::to_string(b) + " is out of bounds.");
        } else {
            res.emplace_back(std::min(a, b), std::max(a, b), w);
        }

    }

    std::sort(res.begin(), res.end(), sortByBoth);

    for (unsigned int i = 0; i < res.size() - 1; i++) {
        if (std::get<0>(res[i]) == std::get<0>(res[i + 1]) && std::get<1>(res[i]) == std::get<1>(res[i + 1])) {
            throw std::runtime_error("Duplicate edge definition at: " + std::to_string(std::get<0>(res[i])) + " " +
                                     std::to_string(std::get<1>(res[i])));
        }
    }

    return res;
}

std::tuple<int, int, double> parseEdge(const std::string &edge_string) {
    std::vector<std::string> edge = splitString(edge_string, ' ');
    int a, b;
    double w;

    if (edge.size() != 3) {
        throw std::runtime_error(
                "Invalid edge definition. " + std::to_string(edge.size()) + " elements provided, 3 expected.");
    } else {
        a = stringToInt(edge[0]);
        b = stringToInt(edge[1]);
        w = stringToDouble(edge[2]);
    }


    return std::make_tuple(a, b, w);
}

bool checkMcEdgelist(const std::vector<std::tuple<int, int, double>> &edgelist) {
    for (auto edge: edgelist) {
        int u = std::get<0>(edge);
        int v = std::get<1>(edge);
        if (u == v) {
            throw std::runtime_error("Selfloop detected at: " + std::to_string(u) + ".");
        }
        double w = std::get<2>(edge);
        if (w == 0) {
            throw std::runtime_error("Zero weight edge: (" +
                                     std::to_string(u) + ", " + std::to_string(v) + ").");
        }
    }
    return true;
}

McObj fileToMcObj(const std::string &path) {
    std::vector<std::string> file_vec = fileToVector(path);
    auto [comments, non_comment_part] = parseComments(file_vec);
    auto [n_nodes, n_edges, edgelist] = parseHeader(non_comment_part);
    std::vector<std::tuple<int, int, double>> edgelist_tuple = parseEdgelist(edgelist, 1, static_cast<int>(n_nodes));

    if (edgelist_tuple.size() != n_edges) {
        throw std::runtime_error("Number of edges incorrect. Found " + std::to_string(edgelist_tuple.size())
                                 + ", but header says " + std::to_string(n_edges) + ".");
    }

    checkMcEdgelist(edgelist_tuple);

    return {n_nodes, n_edges, edgelist_tuple, comments};
}


std::vector<std::string> splitString(std::string input, char delimiter) {
    std::vector<std::string> res = std::vector<std::string>();

    ulong p;
    while ((p = input.find(delimiter)) != std::string::npos) {
        res.push_back(input.substr(0, p));
        input.erase(0, p + 1);
    }

    res.push_back(input);

    return res;
}

int stringToInt(const std::string &input) {
    std::regex integer = std::regex("-?\\d*");
    std::smatch m;
    std::regex_search(input, m, integer);

    if (m.empty() || m.length() != (long) input.length()) {
        throw std::runtime_error("Non valid integer: " + input);
    }

    return std::stoi(input);
}

double stringToDouble(const std::string &input) {
    std::regex integer = std::regex("-?\\d*.\\d*");
    std::smatch m;
    std::regex_search(input, m, integer);

    if (m.empty() || m.length() != (long) input.length()) {
        throw std::runtime_error("Non valid decimal: " + input);
    }

    return std::stod(input);
}

McObj::McObj(unsigned int n, unsigned int m, const std::vector<std::tuple<int, int, double>> &e,
             const std::vector<std::string> &c) {
    nodes = n;
    edges = m;
    edge_list = e;
    comments = c;
}


BqObj fileToBqObj(const std::string &path) {
    std::vector<std::string> file_vec = fileToVector(path);
    auto [comments, non_comment_part] = parseComments(file_vec);
    auto [n_nodes, n_edges, edgelist] = parseHeader(non_comment_part);
    std::vector<std::tuple<int, int, double>> edgelist_tuple = parseEdgelist(edgelist, 1, static_cast<int>(n_nodes));

    if (edgelist_tuple.size() != n_edges) {
        throw std::runtime_error("Number of nonzeros incorrect. " + std::to_string(edgelist_tuple.size()) +
                                 " nonzeros found, but header says " + std::to_string(n_edges));
    }

    if (!checkBqEdgelist(edgelist_tuple)) {
        throw std::runtime_error("List of nonzeros not correct");
    }

    return BqObj{n_nodes, n_edges, edgelist_tuple, comments};
}

NetworKit::Graph bqFileToGraph(const std::string &path) {
    BqObj obj = fileToBqObj(path);
    NetworKit::Graph g = NetworKit::Graph(obj.nodes + 1, true);


    auto diagEntries = std::vector<NetworKit::edgeweight>(g.numberOfNodes(), 0);
    for (auto [a, b, w]: obj.edge_list) {
        if (a != b) {
            NetworKit::edgeweight curWeight = 0;
            if (g.hasEdge(a, b)) {
                curWeight = g.weight(a, b);
            }
            g.addEdge(a, b, curWeight + w);
        } else {
            assert(diagEntries[a] == 0);
            diagEntries[a] = w;
        }
    }

    for (auto u: g.nodeRange()) {
        g.setWeight(0, u, -2 * diagEntries[u] - g.weightedDegree(u));
    }
    g.removeEdge(0, 0);


    return g;
}

bool checkBqEdgelist(const std::vector<std::tuple<int, int, double>> &edgelist) {
    for (auto edge: edgelist) {
        // check for 0 weights
        if (std::get<2>(edge) == 0) {
            throw std::runtime_error("Zero weight detected at edge: " + std::to_string(std::get<0>(edge)) + " " +
                                     std::to_string(std::get<1>(edge)));
        }
    }
    return true;
}

NetworKit::Graph edgelistToGraph(int numNodes, const std::vector<std::tuple<int, int, double>> &edges) {
    NetworKit::Graph g = NetworKit::Graph(numNodes + 1, true);

    for (auto [a, b, w]: edges) {
        g.addEdge(a, b, w);
    }

    return g;
}

NetworKit::Graph mcFileToGraph(const std::string &path) {
    McObj obj = fileToMcObj(path);
    return edgelistToGraph(static_cast<int>(obj.nodes), obj.edge_list);
}

BqObj::BqObj(unsigned int n, unsigned int m, const std::vector<std::tuple<int, int, double>> &e,
             const std::vector<std::string> &c) {
    nodes = n;
    edges = m;
    edge_list = e;
    comments = c;
}


void graphToMcFile(const NetworKit::Graph &g, const std::string &path) {
    std::ofstream file(path);

    if (!g.checkConsistency())
        throw std::runtime_error("Graph is inconsistent.");

    if (file.is_open()) {
        file << g.upperNodeIdBound() - 1 << " " << g.numberOfEdges() << std::endl;
        for (auto e: g.edgeWeightRange()) {
            assert(e.u <= e.v);

            if (e.u == 0 || e.v == 0)
                throw std::runtime_error(
                        "Illegal root edge found for node " + std::to_string(e.u) + ".");

            if (e.weight == 0)
                throw std::runtime_error(
                        "Zero weight detected at edge: " + std::to_string(e.u) + " " + std::to_string(e.v));

            if (e.v == e.u)
                throw std::runtime_error("Selfloop detected at node: " + std::to_string(e.u));

            file << e.v << " " << e.u << " " << e.weight << std::endl;
        }
    } else {
        throw std::runtime_error("File could not be opened: " + path + ".");
    }
    file.close();
}

/*
 * Save a graph as bq file.
 *
 * Selfloops and 0 weights will error.
 * Edges of type (0, v) will be writen as selfloops (v, v)
 * */
void graphToBqFile(const NetworKit::Graph &g, const std::string &path) {
    std::ofstream file(path);

    if (!g.checkConsistency())
        throw std::runtime_error("Graph is inconsistent.");

    if (file.is_open()) {
        file << g.numberOfNodes() - 1 << " " << g.numberOfEdges()
             << std::endl; // subtract 1 from number of nodes, as 0 is only an auxiliary node
        for (auto e: g.edgeWeightRange()) {
            assert(e.u <= e.v);

            if (e.weight == 0)
                throw std::runtime_error(
                        "Zero weight detected at edge: " + std::to_string(e.u) + " " + std::to_string(e.v));

            if (e.v == e.u)
                throw std::runtime_error("Selfloop detected at node: " + std::to_string(e.u));

            if (e.u == 0)
                file << e.v << " " << e.v << " " << e.weight << std::endl;
            else
                file << e.v << " " << e.u << " " << e.weight << std::endl;
        }
    } else {
        throw std::runtime_error("File could not be opened: " + path + ".");
    }
    file.close();
}