#ifndef SMS_IO_HPP
#define SMS_IO_HPP

#include <vector>
#include <tuple>
#include <regex>
#include <fstream>

#include "networkit/graph/Graph.hpp"

NetworKit::Graph test_graph();

class McObj
{
public:
    unsigned int edges;
    unsigned int nodes;
    std::vector<std::string> comments;
    std::vector<std::tuple<int, int, double>> edge_list;

    McObj(unsigned int, unsigned int, const std::vector<std::tuple<int, int, double>> &,
          const std::vector<std::string> &);
};

McObj fileToMcObj(const std::string &path);

class BqObj
{
public:
    unsigned int edges;
    unsigned int nodes;
    std::vector<std::string> comments;
    std::vector<std::tuple<int, int, double>> edge_list;

    BqObj(unsigned int, unsigned int, const std::vector<std::tuple<int, int, double>> &,
          const std::vector<std::string> &);
};

BqObj fileToBqObj(const std::string &path);

NetworKit::Graph bqFileToGraph(const std::string &path);

int stringToInt(const std::string &input);

double stringToDouble(const std::string &input);

bool sortByBoth(const std::tuple<int, int, double> &a, const std::tuple<int, int, double> &b);

std::vector<std::string> fileToVector(const std::string &path);

std::tuple<std::vector<std::string>, std::vector<std::string>>
parseComments(const std::vector<std::string> &file_vector);

std::tuple<unsigned int, unsigned int, std::vector<std::string>> parseHeader(std::vector<std::string> file_vector);

std::vector<std::tuple<int, int, double>> parseEdgelist(const std::vector<std::string> &edgelist, int lower, int upper);

std::tuple<int, int, double> parseEdge(const std::string &edge_string);

bool checkMcEdgelist(const std::vector<std::tuple<int, int, double>> &edgelist);

bool checkBqEdgelist(const std::vector<std::tuple<int, int, double>> &edgelist);

std::vector<std::string> splitString(std::string input, char delimiter);

NetworKit::Graph edgelistToGraph(int nodes, const std::vector<std::tuple<int, int, double>> &edges);

NetworKit::Graph mcFileToGraph(const std::string &path);

void graphToMcFile(const NetworKit::Graph &g, const std::string &path);

void graphToBqFile(const NetworKit::Graph &g, const std::string &path);

#endif //SMS_IO_HPP
