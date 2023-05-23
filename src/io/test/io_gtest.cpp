#include <gtest/gtest.h>

#include "sms/io/io.hpp"


TEST(IO, BqObj) {
    auto bq = BqObj(5, 10, {std::make_tuple(1, 2, 3.1), std::make_tuple(3, 2, -0.1), std::make_tuple(4, 5, 0.9)},
                    {"Test", "Comment"});
    ASSERT_EQ(bq.nodes, 5);
    ASSERT_EQ(bq.edges, 10);
    ASSERT_EQ(bq.edge_list[0], std::make_tuple(1, 2, 3.1));
    ASSERT_EQ(bq.comments[1], "Comment");
}


TEST(IO, ParseComments) {
    auto [c, r] = parseComments(std::vector<std::string>{"#abc", "1 0", "1 2 3"});
    ASSERT_EQ(c, (std::vector<std::string>{"#abc"}));
    ASSERT_EQ(r, (std::vector<std::string>{"1 0", "1 2 3"}));

    ASSERT_ANY_THROW(parseComments(std::vector<std::string>{"#abc", "1 0", "#1 2 3"}));
}


TEST(IO, SplitString) {
    auto v = splitString("a bfd 45 !z", ' ');
    ASSERT_EQ(v, (std::vector<std::string>{"a", "bfd", "45", "!z"}));
}

TEST(IO, ParseHeader) {
    auto [n, m, r] = parseHeader(std::vector<std::string>{"2 1", "1 2 3"});
    ASSERT_EQ(n, 2);
    ASSERT_EQ(m, 1);

    ASSERT_ANY_THROW(parseHeader(std::vector<std::string>{"2 2 1", "1 2 3"}));
}


TEST(IO, ParseEdge) {
    auto [a, b, w] = parseEdge("3 4 0.5");
    ASSERT_EQ(a, 3);
    ASSERT_EQ(b, 4);
    ASSERT_EQ(w, 0.5);

    auto [a_2, b_2, w_2] = parseEdge("67 1 -1");
    ASSERT_EQ(a_2, 67);
    ASSERT_EQ(b_2, 1);
    ASSERT_EQ(w_2, -1.0);

    ASSERT_ANY_THROW(parseEdge("2.1 4 4"));
    ASSERT_ANY_THROW(parseEdge("5 4"));
    ASSERT_ANY_THROW(parseEdge("A B 5.6"));
    ASSERT_ANY_THROW(parseEdge("1  3   4"));
}

TEST(IO, ParseEdgelist) {
    auto v = parseEdgelist(std::vector<std::string>{"3 2 0.3", "1 3 0.4", "1 2 -3"}, 1, 3);
    ASSERT_EQ(v[0], std::make_tuple(1, 2, -3.0));
    ASSERT_EQ(v[1], std::make_tuple(1, 3, 0.4));
    ASSERT_EQ(v[2], std::make_tuple(2, 3, 0.3));

    ASSERT_ANY_THROW(parseEdgelist(std::vector<std::string>{"3 2 0.3", "1 3 0.4", "1 2 -3"}, 1, 2));
    ASSERT_ANY_THROW(parseEdgelist(std::vector<std::string>{"3 2 0.3", "1 3 0.4", "1 2 -3", "2 1 4"}, 1, 3));
    ASSERT_ANY_THROW(parseEdgelist(std::vector<std::string>{"3 2 0.3", "1 3 0.4", "1 0 -3"}, 1, 3));
}

TEST(IO, CheckBqEdgelist) {
    ASSERT_TRUE(checkBqEdgelist(
            std::vector<std::tuple<int, int, double>>{std::make_tuple(1, 2, -3.0), std::make_tuple(1, 3, 0.4),
                                                      std::make_tuple(2, 3, 0.3)}));
    ASSERT_ANY_THROW(checkBqEdgelist(
            std::vector<std::tuple<int, int, double>>{std::make_tuple(1, 2, -3.0), std::make_tuple(1, 3, 0.4),
                                                      std::make_tuple(2, 3, 0.3), std::make_tuple(1, 1, 0)}));
}

TEST(FileToBq, ZeroWeights) {
    ASSERT_ANY_THROW(fileToBqObj("test/data/error_0_entry.bq"));
}

TEST(FileToBq, Comment) {
    ASSERT_ANY_THROW(fileToBqObj("test/data/error_comment.bq"));
}

TEST(FileToBq, CommentInFile) {
    ASSERT_ANY_THROW(fileToBqObj("test/data/error_comment_in_file.bq"));
}

TEST(FileToBq, Delimiter) {
    ASSERT_ANY_THROW(fileToBqObj("test/data/error_delimiter.bq"));
}

TEST(FileToBq, HeaderMissing) {
    ASSERT_ANY_THROW(fileToBqObj("test/data/error_header_missing.bq"));
}

TEST(FileToBq, DoubleDef1) {
    ASSERT_ANY_THROW(fileToBqObj("test/data/error_double_definition_1.bq"));
}

TEST(FileToBq, DoubleDef2) {
    ASSERT_ANY_THROW(fileToBqObj("test/data/error_double_definition_2.bq"));
}

TEST(FileToBq, DoubleDef3) {
    ASSERT_ANY_THROW(fileToBqObj("test/data/error_double_definition_3.bq"));
}

TEST(FileToBq, NoWeight) {
    ASSERT_ANY_THROW(fileToBqObj("test/data/error_no_weight.bq"));
}

TEST(FileToBq, NodeIdLetter) {
    ASSERT_ANY_THROW(fileToBqObj("test/data/error_id_letter.bq"));
}

TEST(FileToBq, NodeIdOutOfBounds) {
    ASSERT_ANY_THROW(fileToBqObj("test/data/error_id_range_0.bq"));
    ASSERT_ANY_THROW(fileToBqObj("test/data/error_id_range_5.bq"));
}

TEST(FileToBq, EdgesNumber) {
    ASSERT_ANY_THROW(fileToBqObj("test/data/error_too_few_entries.bq"));
    ASSERT_ANY_THROW(fileToBqObj("test/data/error_too_many_entries.bq"));
}

TEST(FileToBq, NoEntries) {
    ASSERT_ANY_THROW(fileToBqObj("test/data/error_no_entries.bq"));
}

TEST(FileToBq, NoFile) {
    ASSERT_ANY_THROW(fileToBqObj("not/existing/file"));
}

TEST(FileToBq, Correct) {
    auto obj = fileToBqObj("test/data/square.bq");

    ASSERT_EQ(obj.comments,
              (std::vector<std::string>{"# Some comments", "# for testing", "# this file is a square graph"}));
    ASSERT_EQ(obj.edge_list,
              (std::vector<std::tuple<int, int, double>>{std::make_tuple(1, 2, 0.5), std::make_tuple(1, 4, -1.2),
                                                         std::make_tuple(2, 3, -2), std::make_tuple(3, 4, 0.6),
                                                         std::make_tuple(4, 4, 0.5)}));
    ASSERT_EQ(obj.edges, 5);
    ASSERT_EQ(obj.nodes, 4);
}


TEST(BasicFunctions, StringToInt) {
    ASSERT_ANY_THROW(stringToInt("xab"));
    ASSERT_ANY_THROW(stringToInt("1.0"));
    ASSERT_ANY_THROW(stringToInt("-0.1"));
    ASSERT_ANY_THROW(stringToInt("--0.1"));
    ASSERT_EQ(stringToInt("56"), 56);
    ASSERT_EQ(stringToInt("-42"), -42);
}

TEST(BasicFunctions, StringToDouble) {
    ASSERT_ANY_THROW(stringToDouble("xab"));
    ASSERT_ANY_THROW(stringToDouble("--3"));
    ASSERT_EQ(stringToDouble("56"), 56);
    ASSERT_EQ(stringToDouble("-42"), -42);
    ASSERT_EQ(stringToDouble("56.58"), 56.58);
    ASSERT_EQ(stringToDouble("-42.675"), -42.675);
    ASSERT_EQ(stringToDouble("1"), 1);
}

TEST(IO, McObj) {
    auto mc = McObj(5, 10, {std::make_tuple(1, 2, 3.1), std::make_tuple(3, 2, -0.1), std::make_tuple(4, 5, 0.9)},
                    {"Test", "Comment"});
    ASSERT_EQ(mc.nodes, 5);
    ASSERT_EQ(mc.edges, 10);
    ASSERT_EQ(mc.edge_list[0], std::make_tuple(1, 2, 3.1));
    ASSERT_EQ(mc.comments[1], "Comment");
}


TEST(IO, FileToVector) {
    auto v = fileToVector("test/data/simple_file.txt");
    ASSERT_EQ(v, (std::vector{std::string("a"), std::string("b c"), std::string("def")}));
    ASSERT_ANY_THROW(fileToVector("non/existing/file"));
}


TEST(IO, DISABLED_CheckMcEdgelist) {
    ASSERT_TRUE(checkMcEdgelist(
            std::vector<std::tuple<int, int, double>>{std::make_tuple(1, 2, -3.0), std::make_tuple(1, 3, 0.4),
                                                      std::make_tuple(2, 3, 0.3)}));
    ASSERT_ANY_THROW(checkMcEdgelist(
            std::vector<std::tuple<int, int, double>>{std::make_tuple(1, 2, -3.0), std::make_tuple(1, 3, 0.4),
                                                      std::make_tuple(2, 3, 0.3), std::make_tuple(1, 1, 4)}));
}

TEST(FileToMc, ZeroWeights) {
    ASSERT_ANY_THROW(fileToMcObj("test/data/error_0_weights.mc"));
}

TEST(FileToMc, Comment) {
    ASSERT_ANY_THROW(fileToMcObj("test/data/error_comment.mc"));
}

TEST(FileToMc, CommentInFile) {
    ASSERT_ANY_THROW(fileToMcObj("test/data/error_comment_in_file.mc"));
}

TEST(FileToMc, Delimiter) {
    ASSERT_ANY_THROW(fileToMcObj("test/data/error_delimiter.mc"));
}

TEST(FileToMc, HeaderMission) {
    ASSERT_ANY_THROW(fileToMcObj("test/data/error_header_missing.mc"));
}

TEST(FileToMc, DoubleDef) {
    ASSERT_ANY_THROW(fileToMcObj("test/data/error_double_definition.mc"));
}

TEST(FileToMc, NoWeight) {
    ASSERT_ANY_THROW(fileToMcObj("test/data/error_no_weight.mc"));
}

TEST(FileToMc, NodeIdLetter) {
    ASSERT_ANY_THROW(fileToMcObj("test/data/error_node_id_letter.mc"));
}

TEST(FileToMc, NodeIdOutOfBounds) {
    ASSERT_ANY_THROW(fileToMcObj("test/data/error_node_id_range_0.mc"));
    ASSERT_ANY_THROW(fileToMcObj("test/data/error_node_id_range_5.mc"));
}

TEST(FileToMc, Selfloop) {
    ASSERT_ANY_THROW(fileToMcObj("test/data/error_selfloop.mc"));
}

TEST(FileToMc, EdgesNumber) {
    ASSERT_ANY_THROW(fileToMcObj("test/data/error_too_few_edges.mc"));
    ASSERT_ANY_THROW(fileToMcObj("test/data/error_too_many_edges.mc"));
}

TEST(FileToMc, NoFile) {
    ASSERT_ANY_THROW(fileToMcObj("not/existing/file"));
}

TEST(FileToMc, Correct) {
    auto obj = fileToMcObj("test/data/square.mc");

    ASSERT_EQ(obj.comments,
              (std::vector<std::string>{"# Some comments", "# for testing", "# this file is a square graph",
                                        "# maxcut value: 1.1"}));
    ASSERT_EQ(obj.edge_list,
              (std::vector<std::tuple<int, int, double>>{std::make_tuple(1, 2, 0.5), std::make_tuple(1, 4, -1.2),
                                                         std::make_tuple(2, 3, -2), std::make_tuple(3, 4, 0.6)}));
    ASSERT_EQ(obj.edges, 4);
    ASSERT_EQ(obj.nodes, 4);
}


TEST(FileToGraph, EdgelistToGraph) {
    auto g = edgelistToGraph(4, std::vector<std::tuple<int, int, double>>{std::make_tuple(1, 2, 0.5),
                                                                          std::make_tuple(1, 4, -1.2),
                                                                          std::make_tuple(2, 3, -2),
                                                                          std::make_tuple(3, 4, 0.6)});
    ASSERT_EQ(g.numberOfNodes(), 5);
    ASSERT_EQ(g.numberOfEdges(), 4);
    ASSERT_TRUE(g.hasEdge(1, 2));
    ASSERT_TRUE(g.hasEdge(1, 4));
    ASSERT_TRUE(g.hasEdge(2, 3));
    ASSERT_TRUE(g.hasEdge(3, 4));
}

TEST(FileToGraph, McFileToGraph) {
    auto g = mcFileToGraph("test/data/square.mc");
    ASSERT_EQ(g.numberOfNodes(), 5);
    ASSERT_EQ(g.numberOfEdges(), 4);
    ASSERT_TRUE(g.hasEdge(1, 2));
    ASSERT_TRUE(g.hasEdge(2, 3));
    ASSERT_TRUE(g.hasEdge(3, 4));
    ASSERT_TRUE(g.hasEdge(1, 4));
}

TEST(FileToGraph, McFileToGraphError) {
    ASSERT_ANY_THROW(mcFileToGraph("test/data/error_too_few_edges.mc"));
}

TEST(GraphToFile, SimpleSaveTest) {
    auto orig = mcFileToGraph("test/data/square.mc");
    graphToMcFile(orig, "/tmp/test.mc");

    auto g = mcFileToGraph("/tmp/test.mc");
    ASSERT_EQ(g.numberOfNodes(), 5);
    ASSERT_EQ(g.numberOfEdges(), 4);
    ASSERT_TRUE(g.hasEdge(1, 2));
    ASSERT_TRUE(g.hasEdge(2, 3));
    ASSERT_TRUE(g.hasEdge(3, 4));
    ASSERT_TRUE(g.hasEdge(4, 1));
}

TEST(GraphToFile, ComplexSaveTest) {
    auto orig = mcFileToGraph("test/data/mannino_k487b.mc");
    graphToMcFile(orig, "/tmp/test.mc");

    auto g = mcFileToGraph("/tmp/test.mc");
    ASSERT_EQ(g.numberOfNodes(), orig.numberOfNodes());
    ASSERT_EQ(g.numberOfEdges(), orig.numberOfEdges());

    for (auto e: orig.edgeWeightRange()) {
        ASSERT_TRUE(g.hasEdge(e.u, e.v));
        ASSERT_EQ(g.weight(e.u, e.v), e.weight);
    }
}


TEST(GraphToFile, ErrorSaveSunEdge) {
    NetworKit::Graph orig(2, true);
    orig.addEdge(0, 1, 0);

    ASSERT_ANY_THROW(graphToMcFile(orig, "/tmp/test.mc"));
}

TEST(GraphToFile, ErrorSaveTestZeroWeight) {
    NetworKit::Graph orig(3, true);
    orig.addEdge(1, 2, 0);

    ASSERT_ANY_THROW(graphToMcFile(orig, "/tmp/test.mc"));
}

TEST(GraphToFile, ErrorSaveTestSelfloop) {
    NetworKit::Graph orig(2, true);
    orig.addEdge(1, 1, 1);

    ASSERT_ANY_THROW(graphToMcFile(orig, "/tmp/test.mc"));
}

TEST(GraphToFile, DoubleEdgeTest) {
    NetworKit::Graph orig(3, true);
    orig.addEdge(1, 2, 1);
    orig.addEdge(1, 2, 1);

    ASSERT_ANY_THROW(graphToMcFile(orig, "/tmp/test.mc"));
}

TEST(GraphToFile, DoubleEdgeTest2) {
    NetworKit::Graph orig(3, true);
    orig.addEdge(1, 2, 1);
    orig.addEdge(2, 1, 1);

    ASSERT_ANY_THROW(graphToMcFile(orig, "/tmp/test.mc"));
}

TEST(GraphToFile, DoubleEdgeTest3) {
    NetworKit::Graph orig(3, true);
    orig.addEdge(1, 2, 1);
    orig.addEdge(2, 1, 0);

    ASSERT_ANY_THROW(graphToMcFile(orig, "/tmp/test.mc"));
}


TEST(FileToGraph, BqFileToGraph1) {
    auto g = bqFileToGraph("test/data/square.bq");
    ASSERT_EQ(g.numberOfNodes(), 5);
    ASSERT_EQ(g.numberOfEdges(), 8);
    ASSERT_TRUE(g.hasEdge(1, 2));
    ASSERT_TRUE(g.hasEdge(1, 4));
    ASSERT_TRUE(g.hasEdge(2, 3));
    ASSERT_TRUE(g.hasEdge(3, 4));
    ASSERT_TRUE(g.hasEdge(0, 4));
}

TEST(FileToGraph, BqFileToGraph2) {
    auto g = bqFileToGraph("test/data/three.bq");
    ASSERT_EQ(g.numberOfNodes(), 4);
    ASSERT_EQ(g.numberOfEdges(), 5);
    ASSERT_TRUE(g.hasEdge(1, 2));
    ASSERT_TRUE(g.hasEdge(2, 3));
    ASSERT_TRUE(g.hasEdge(0, 1));
    ASSERT_TRUE(g.hasEdge(0, 2));
    ASSERT_TRUE(g.hasEdge(0, 3));

    ASSERT_EQ(g.weight(0, 3), 4);
}

TEST(FileToGraph, BqFileToGraphError) {
    ASSERT_ANY_THROW(mcFileToGraph("test/data/error_too_few_edges.bq"));
}


TEST(GraphToFileBq, SimpleSaveTest) {
    auto orig = bqFileToGraph("test/data/square.bq");
    graphToBqFile(orig, "/tmp/test2.bq");

    auto g = bqFileToGraph("/tmp/test2.bq");
    ASSERT_EQ(g.numberOfNodes(), 5);
    ASSERT_EQ(g.numberOfEdges(), 8);
    ASSERT_TRUE(g.hasEdge(1, 2));
    ASSERT_TRUE(g.hasEdge(1, 4));
    ASSERT_TRUE(g.hasEdge(2, 3));
    ASSERT_TRUE(g.hasEdge(3, 4));
    ASSERT_TRUE(g.hasEdge(0, 4));
}


TEST(GraphToFileBq, SimpleSaveTest2) {
    NetworKit::Graph orig(10, true);
    orig.addEdge(1, 0, 1);
    orig.addEdge(2, 0, 1);
    orig.addEdge(0, 3, 1);
    orig.addEdge(1, 2, 1);
    orig.addEdge(3, 2, 1);
    orig.addEdge(3, 4, 1);
    orig.addEdge(5, 4, 1);
    orig.addEdge(5, 6, 1);
    orig.addEdge(7, 6, 1);
    orig.addEdge(7, 8, 1);
    orig.addEdge(9, 8, 1);
    graphToBqFile(orig, "/tmp/test.bq");

    auto g = bqFileToGraph("/tmp/test.bq");
    ASSERT_EQ(g.numberOfNodes(), 10);
    ASSERT_EQ(g.numberOfEdges(), 17);
}

TEST(GraphToFileBq, ErrorSaveTestZeroWeight) {
    NetworKit::Graph orig(2, true);
    orig.addEdge(0, 1, 0);

    ASSERT_ANY_THROW(graphToBqFile(orig, "/tmp/test.bq"));
}

TEST(GraphToFileBq, ErrorSaveTestSelfloop) {
    NetworKit::Graph orig(2, true);
    orig.addEdge(1, 1, 1);

    ASSERT_ANY_THROW(graphToBqFile(orig, "/tmp/test.mc"));
}

TEST(GraphToFileBq, DoubleEdgeTest) {
    NetworKit::Graph orig(2, true);
    orig.addEdge(1, 0, 1);
    orig.addEdge(1, 0, 1);

    ASSERT_ANY_THROW(graphToBqFile(orig, "/tmp/test.mc"));
}

TEST(GraphToFileBq, DoubleEdgeTest2) {
    NetworKit::Graph orig(2, true);
    orig.addEdge(1, 0, 1);
    orig.addEdge(0, 1, 1);

    ASSERT_ANY_THROW(graphToBqFile(orig, "/tmp/test.mc"));
}

TEST(GraphToFileBq, DoubleEdgeTest3) {
    NetworKit::Graph orig(2, true);
    orig.addEdge(1, 0, 1);
    orig.addEdge(0, 1, 0);

    ASSERT_ANY_THROW(graphToBqFile(orig, "/tmp/test.mc"));
}
