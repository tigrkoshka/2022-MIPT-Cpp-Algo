// https://leetcode.com/problems/critical-connections-in-a-network/
//
// There are n servers numbered from 0 to n - 1 connected by undirected
// server-to-server connections forming a network where
// connections[i] = [a_i, b_i] represents a connection between
// servers a_i and b_i. Any server can reach other servers directly or
// indirectly through the network.
//
// A critical connection is a connection that, if removed, will make
// some servers unable to reach some other server.
//
// Return all critical connections in the network in any order.

class Solution {
private:
    using Vertex = int;
    using Edge = vector<Vertex>;
    using Edges = vector<Edge>;

    class Graph {
    private:
        // AdjacencyList[i] represents all edges from the i-th vertex
        using AdjacencyLists = vector<vector<Vertex>>;
        using Depth = uint32_t;

    private:
        AdjacencyLists adjLists;

    private:
        // Traverses all vertices of the graph connected to curr and
        // adds those edges which are not contained in any cycle to
        // the provided nonCycle container.
        //
        // The return value is always less or equal to the specified currDepth.
        // If the return value differs from the specified one,
        // the current vertex is in a cycle.
        // The reverse claim is not always true.
        // The exact return value is an implementation detail
        // and should not be counted upon.
        Depth getNonCycleDFS(Vertex curr,
                             Depth currDepth,
                             Vertex currParent,
                             Edges &nonCycle,
                             vector<Depth> &depths) const {
            // We have not yet noticed a cycle, so prepare to return
            // the specified value of the depth
            depths[curr] = currDepth;

            for (auto child: adjLists[curr]) {
                // Because our graph is undirected, there is always an edge
                // connecting the current vertex to the one we came from,
                // which we are not interested in
                if (child == currParent) {
                    continue;
                }

                // If he child has not yet been visited, recurse down to it
                // with a guess for the child's depth being one more than
                // the current depth
                if (depths[child] == UINT32_MAX) {
                    depths[child] = getNonCycleDFS(child,
                                                   currDepth + 1,
                                                   curr,
                                                   nonCycle,
                                                   depths);
                }

                // Override the depth of the current vertex if its child has
                // a smaller depth. That means that both this vertex and
                // the child are in the same cycle.
                depths[curr] = min(depths[curr], depths[child]);

                // If the child's depth equals the one initially
                // provided for it, then the edge containing the current vertex
                // with the child is not in a cycle. Otherwise, that means that
                // either the child has already been visited and therefore
                // there is a cycle containing the current edge,
                // or it is connected to a vertex closer to the initial one
                // than that the current vertex, so again, there is a cycle
                // containing the current edge.
                if (depths[child] == currDepth + 1) {
                    nonCycle.push_back({curr, child});
                }
            }

            return depths[curr];
        }

        void addEdge(const Edge &edge) {
            adjLists[edge[0]].push_back(edge[1]);
            adjLists[edge[1]].push_back(edge[0]);
        }

    public:
        Graph(size_t n, const Edges &edges) : adjLists(n) {
            for (const auto &edge: edges) {
                addEdge(edge);
            }
        }

        // Traverses all vertices of the graph and
        //
        //   - adds those of them which are not contained in any cycle
        //     and are not recorded in the cycle set to nonCycle
        //
        //   - adds the rest of the edges to cycle
        Edges getNonCycle() const {
            Edges nonCycle;
            vector<Depth> depths(adjLists.size(), UINT32_MAX);

            getNonCycleDFS(0, 0, UINT32_MAX, nonCycle, depths);

            return nonCycle;
        }
    };

public:
    // The idea of the algorithm is that in an undirected graph those and only
    // those edges are critical which are not contained in any cycle.
    //
    // Proof:
    // We are going to prove an equivalent claim: in an undirected graph those
    // and only those edges are non-critical which are contained in a cycle.
    //
    // If an edge is non-critical then let's consider the two vertices that it
    // connects. Because the edge is non-critical, if it was removed, those two
    // vertices would be still connected by come path in the resulting graph.
    // That path together with the edge in question, therefore, form a cycle
    // in the initial graph.
    //
    // Not let's assume an edge connecting the vertices A and B is in a cycle.
    // Then let's consider the paths which connect the i-th vertex to the j-th
    // for each i and j. Such paths exist for each i and j, because per
    // the condition of the problem the initial graph is directed. Those of them
    // which do not include the edge in question will not be affected by
    // the edge removal. As for those which do contain the edge, they consist
    // of 3 parts: a path from the i-th vertex to A, the edge in question
    // and a path from B to the j-th vertex. If the edge is removed, we can
    // replace it with the rest of the cycle it is in, thus creating paths
    // connecting the i-th vertex to the j-th. Therefore, the graph without
    // the edge in question is still connected and the edge itself is
    // non-critical, which concludes the proof.
    static Edges criticalConnections(size_t n, const Edges &connections) {
        Graph graph(n, connections);
        return graph.getNonCycle();
    }
};