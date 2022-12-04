#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

namespace po = boost::program_options;

bool parseCommandLine(int argc, char **argv,
                      std::string &input,
                      std::string &outputGraph,
                      std::string &outputMST,
                      std::string &outputEdges) {
    namespace fs = boost::filesystem;

    try {
        po::options_description desc("Usage: grapher [options...]\n\nOptions", 120);
        desc.add_options()
                ("input,i", po::value<std::string>(&input)->required(), "File containing the graph (required)")
                ("output-graph,g", po::value<std::string>(&outputGraph), "File to store the graph in DOT notation\n(default: graph.dot in the same directory as input)")
                ("output-mst,m", po::value<std::string>(&outputMST), "File to store the MST in DOT notation\n(default: mst.dot in the same directory as input)")
                ("output-edges,e", po::value<std::string>(&outputEdges), "File to store the critical and pseudo critical edges in DOT notation\n(default: edges.dot in the same directory as input)")
                ("help", "Print usage and exit");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        if (vm.count("help") > 0) {
            std::cout << desc << "\n";
            return false;
        }

        po::notify(vm);

        fs::path dir = fs::absolute(fs::path(input)).parent_path();

        if (outputGraph.empty()) {
            outputGraph = (dir / "graph.dot").string();
        } else {
            fs::create_directories(fs::absolute(fs::path(outputGraph)).parent_path());
        }

        if (outputMST.empty()) {
            outputMST = (dir / "mst.dot").string();
        } else {
            fs::create_directories(fs::absolute(fs::path(outputMST)).parent_path());
        }

        if (outputEdges.empty()) {
            outputEdges = (dir / "edges.dot").string();
        } else {
            fs::create_directories(fs::absolute(fs::path(outputEdges)).parent_path());
        }
    }
    catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return false;
    }
    catch (...) {
        std::cerr << "Unknown error" << "\n";
        return false;
    }

    return true;
}

class Solution {
private:
    struct VertexProps {
    };

    struct EdgeProps {
        uint64_t weight{};
        std::string color = "black";
    };

    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
    VertexProps, EdgeProps> Graph;
    typedef boost::graph_traits<Graph>::edge_descriptor Edge;

private:
    static std::vector<Edge> mstEdges(const Graph &graph) {
        std::vector<Edge> edges;

        boost::kruskal_minimum_spanning_tree(
                graph, std::back_inserter(edges),
                boost::weight_map(boost::get(&EdgeProps::weight, graph))
        );

        return edges;
    }

    static std::pair<Graph, uint64_t> mstWithWeight(const Graph &graph) {
        auto edges = mstEdges(graph);

        Graph mst(boost::num_vertices(graph));
        uint64_t totalWeight = 0;

        for (const auto &edge: edges) {
            boost::add_edge(boost::source(edge, graph),
                            boost::target(edge, graph),
                            {graph[edge].weight}, mst);

            totalWeight += graph[edge].weight;
        }

        return {mst, totalWeight};
    }

    static bool isConnected(const Graph &graph) {
        std::vector<int> component (boost::num_vertices(graph));
        size_t num_components = boost::connected_components(graph, &component[0]);

        return num_components == 1;
    }

    static uint64_t mstWeight(const Graph &graph) {
        auto edges = mstEdges(graph);

        uint64_t totalWeight = 0;
        for (const auto &edge: edges) {
            totalWeight += graph[edge].weight;
        }

        return totalWeight;
    }

    static Graph withoutEdge(const Graph &graph, const Edge &edge) {
        Graph res = graph;
        boost::remove_edge(boost::source(edge, graph),
                           boost::target(edge, graph),
                           res);

        return res;
    }

    static Graph withEdge(const Graph &graph, const Edge &edge) {
        // We use a trick to enforce the Kruskal's algorithm to use
        // the current edge: we assign it a weight of 0 so that the algorithm
        // includes it on the first iteration, and then we add the weight
        // of the current edge to the resulting MST's weight

        uint32_t from = boost::source(edge, graph);
        uint32_t to = boost::target(edge, graph);

        Graph res = graph;
        boost::remove_edge(from, to, res);
        boost::add_edge(from, to, {0}, res);

        return res;
    }

public:
    static Graph read(const std::string &input) {
        std::ifstream in(input);

        size_t n;
        in >> n;

        Graph graph(n);

        char _;
        in >> _;
        while (in >> _) {
            uint32_t from, to, weight;
            in >> from >> _ >> to >> _ >> weight >> _ >> _;

            boost::add_edge(from, to, {weight}, graph);
        }

        return graph;
    }

    static std::pair<Graph, Graph> mstWithColored(
            const Graph &graph
    ) {
        auto [mst, baseWeight] = mstWithWeight(graph);

        Graph colouredMST(boost::num_vertices(graph));

        auto iters = boost::edges(graph);
        for (auto iter = iters.first; iter != iters.second; ++iter) {
            std::string color;

            // We use a different approach from the one in the simple solution,
            // this one is asymptotically worse, but the other one cannot be
            // implemented by straightforward usage of boost functions
            //
            // We iterate through edges of the graph and check the following:
            //
            //   - if the graph without the current edge is not connected
            //     or if its MST weight is bigger than that of the initial graph
            //     the current edge is critical
            //
            //   - else if the weight of the MST containing the current edge
            //     equals to the weight of the MST of the initial graph, then
            //     the current edge is pseudo critical, because an MST not
            //     containing the current edge exists as implied by the fact
            //     that we didn't pass the previous check (otherwise the weight
            //     of the MST in a graph without the current edge would have
            //     been greater than that of the initial graph)
            //
            //   - else the current edge is redundant (i.e. not contained
            //     in any MST
            Graph noEdge = withoutEdge(graph, *iter);

            if (!isConnected(noEdge) || mstWeight(noEdge) > baseWeight) {
                color = "red";
            } else if (mstWeight(withEdge(graph, *iter)) + graph[*iter].weight == baseWeight) {
                color = "blue";
            } else {
                continue;
            }

            boost::add_edge(boost::source(*iter, graph),
                            boost::target(*iter, graph),
                            {graph[*iter].weight, color},
                            colouredMST);
        }

        return {mst, colouredMST};
    }

    static void printDotGraph(const std::string &outFile, Graph &graph) {
        std::ofstream out(outFile);

        boost::dynamic_properties dp;
        dp.property("color", boost::get(&EdgeProps::color, graph));
        dp.property("weight", boost::get(&EdgeProps::weight, graph));
        dp.property("label", boost::get(&EdgeProps::weight, graph));
        dp.property("node_id", boost::get(boost::vertex_index, graph));
        boost::write_graphviz_dp(out, graph, dp);
    }
};

int main(int argc, char **argv) {
    std::string input, outputGraph, outputMST, outputEdges;

    parseCommandLine(argc, argv, input, outputGraph, outputMST, outputEdges);

    auto graph = Solution::read(input);
    auto [mst, coloredMST] = Solution::mstWithColored(graph);

    Solution::printDotGraph(outputGraph, graph);
    Solution::printDotGraph(outputMST, mst);
    Solution::printDotGraph(outputEdges, coloredMST);

    return 0;
}