// https://leetcode.com/problems/find-critical-and-pseudo-critical-edges-in-minimum-spanning-tree/
//
// Given a weighted undirected connected graph with n vertices numbered
// from 0 to n - 1, and an array edges where edges[i] = [a_i, b_i, weight_i]
// represents a bidirectional and weighted edge between nodes a_i and b_i.
// A minimum spanning tree (MST) is a subset of the graph's edges that connects
// all vertices without cycles and with the minimum possible total edge weight.
//
// Find all the critical and pseudo-critical edges in the given graph's minimum
// spanning tree (MST). An MST edge whose deletion from the graph would cause
// the MST weight to increase is called a critical edge. On the other hand,
// a pseudo-critical edge is that which can appear in some MSTs but not all.
//
// Note that you can return the indices of the edges in any order.

class Solution {
private:
  using Vertex = uint32_t;
  using Weight = uint64_t;
  using Depth = uint32_t;

  struct Edge {
  public:
    int index;
    Vertex from;
    Vertex to;

    Edge(int index_, const vector<int> &edge) : index(index_),
                                                from(edge[0]),
                                                to(edge[1]) {}

    Edge(int index_, Vertex from_, Vertex to_) : index(index_),
                                                 from(from_),
                                                 to(to_) {}

    [[nodiscard]] Edge revert() const {
      return {index, to, from};
    }
  };

  struct PairHash {
    uint64_t operator()(const pair<Vertex, Vertex> &vertices) const {
      return static_cast<uint64_t>(vertices.first) << 32ull |
             vertices.second;
    }
  };

  // UnionFind implements a disjoint set union (DSU) data structure,
  // a clear detailed explanation of which can be found, for example, here:
  // https://en.wikipedia.org/wiki/Disjoint-set_data_structure
  class UnionFind {
  private:
    vector<Vertex> parent;
    vector<Depth> rank;

  public:
    explicit UnionFind(size_t n) : parent(n), rank(n) {
      for (size_t i = 0; i < n; ++i) {
        parent[i] = i;
      }
    }

    Vertex findParent(Vertex vertex) {
      if (vertex == parent[vertex]) {
        return vertex;
      }

      return parent[vertex] = findParent(parent[vertex]);
    }

    void unionSets(Vertex first, Vertex second) {
      first = findParent(first);
      second = findParent(second);

      if (first != second) {
        if (rank[first] < rank[second]) {
          swap(first, second);
        }

        parent[second] = first;

        if (rank[first] == rank[second]) {
          ++rank[first];
        }
      }
    }
  };

  class Graph {
  private:
    // AdjacencyList[i] represents all edges from the i-th vertex
    using AdjacencyLists = vector<vector<Edge>>;
    using EdgesList = vector<Edge>;

  private:
    AdjacencyLists adjLists;
    EdgesList edges;
    vector<Depth> depths;

  private:
    // Traverses all vertices of the graph connected to curr and
    //
    //   - adds those of them which are not contained in any cycle
    //     and are not recorded in the cycle set to nonCycle
    //
    //   - adds the rest of the edges to cycle
    //
    // The return value is always less or equal to the specified currDepth.
    // If the return value differs from the specified one,
    // the current vertex is in a cycle.
    // The reverse claim is not always true.
    // The exact return value is an implementation detail
    // and should not be counted upon.
    Depth categorizeEdgesDFS(Vertex curr,
                             Depth currDepth,
                             Vertex currParent,
                             unordered_set<int> &cycle,
                             unordered_set<int> &nonCycle) {
      // We have not yet noticed a cycle, so prepare to return
      // the specified value of the depth
      depths[curr] = currDepth;

      for (const auto &edge: adjLists[curr]) {
        Vertex child = edge.to;

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
          depths[child] = categorizeEdgesDFS(child,
                                             currDepth + 1,
                                             curr,
                                             cycle,
                                             nonCycle);
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
        if (depths[child] == currDepth + 1 &&
            cycle.find(edge.index) == cycle.end()) {
          nonCycle.insert(edge.index);
        } else {
          cycle.insert(edge.index);
        }
      }

      return depths[curr];
    }

  public:
    explicit Graph(size_t n) : adjLists(n) {}

    void addEdge(const Edge &edge) {
      adjLists[edge.from].push_back(edge);
      adjLists[edge.to].push_back(edge.revert());
      edges.push_back(edge);
    }

    // Traverses all vertices of the graph and
    //
    //   - adds those of them which are not contained in any cycle
    //     and are not recorded in the cycle set to nonCycle
    //
    //   - adds the rest of the edges to cycle
    void categorizeEdges(unordered_set<int> &nonCycle,
                         unordered_set<int> &cycle) {
      depths = vector<Depth>(adjLists.size(), UINT32_MAX);

      // Traverse all edges (and therefore all vertices,
      // because we build the graph from edges, so there are no
      // isolated vertices
      for (const auto &edge: edges) {
        // Skip already visited vertices
        if (depths[edge.from] != UINT32_MAX) {
          continue;
        }

        categorizeEdgesDFS(edge.from, 0, UINT32_MAX,
                           cycle, nonCycle);
      }
    }
  };

  using WeightBins = map<Weight, vector<Edge>>;
  using ConnectedSetsBins = unordered_map<
      pair<Vertex, Vertex>,
      vector<Edge>,
      PairHash
      >;

private:
  // Distributes edges into bins defined by the edges' weights and
  // sorts the bins by weights (because WeightBins is an ordered map).
  static WeightBins edgesToWeightBins(const vector<vector<int>> &edges) {
    WeightBins bins;

    for (size_t i = 0; i < edges.size(); ++i) {
      bins[edges[i][2]].emplace_back(i, edges[i]);
    }

    return bins;
  }

  // Distributes edges into bins defined by the sets from UnionFind
  // that the edges connect.
  //
  // Skips those edges that connect vertices within the same set,
  // because such edges are redundant (cannot be contained in any MST)
  // and we can just consider that they didn't exist in the first place.
  //
  // Proof of the redundancy:
  //
  // If an edge of weight W connects vertices within the same set,
  // that means that on one of the previous iterations we united
  // two sets, one of which contained one end of the edge,
  // and the other contained the second end (because initially all vertices
  // are in different sets). Because we traverse edges in the weight-ascending
  // order, that means that those sets are connected with an edge weighing
  // less than the current one, so if we were to include the current edge
  // in a spanning tree, the resulting total weight of that tree
  // would be greater than that of the one found by the Kruskal's algorithm,
  // therefore, it cannot be a minimum spanning tree.
  //
  // For more details on the Kruskal's algorithm please refer to:
  // https://en.wikipedia.org/wiki/Kruskal%27s_algorithm
  static ConnectedSetsBins edgesToSetsBins(const vector<Edge> &edges,
                                           UnionFind &unionFind) {
    ConnectedSetsBins bins;

    for (const auto &edge: edges) {
      Vertex fromParent = unionFind.findParent(edge.from);
      Vertex toParent = unionFind.findParent(edge.to);

      // Skip edges connecting the same sets
      if (fromParent == toParent) {
        continue;
      }

      // Because our graph is undirected, we need to put edges that
      // connect two sets A and B in the same bin regardless of
      // the recorded orientation of the edge. For that purpose
      // the bins are denoted as [minIndex, maxIndex], where
      // minIndex and maxIndex are the minimum and the maximum
      // of the indices of A and B.
      if (fromParent > toParent) {
        swap(fromParent, toParent);
      }

      bins[{fromParent, toParent}].push_back(edge);
    }

    return bins;
  }

public:
  // The only reason this function's argument's and return value's
  // (and all the respective values throughout the code)
  // template parameter is int and not the more semantically correct
  // uint32_t is because otherwise Leetcode cannot compile this.
  static vector<vector<int>> findCriticalAndPseudoCriticalEdges(
      size_t n, const vector<vector<int>> &allEdges
  ) {
    unordered_set<int> critical, pseudoCritical;

    UnionFind unionFind(n);

    // Sort edges by weights and distribute to weight-bins.
    WeightBins weightBins = edgesToWeightBins(allEdges);

    for (const auto &[weight, edges]: weightBins) {
      // Distribute edges of the same weight to bins by the sets
      // from the UnionFind that are connected by them
      ConnectedSetsBins connectedSetsBins = edgesToSetsBins(edges,
                                                            unionFind);

      // Build the graph where vertices are the sets from UnionFind
      // and there is an edge between vertices if the initial graph
      // contains an edge with the current weight connecting vertices
      // of the initial graph from the respective sets from UnionFind
      Graph graph(n);
      for (const auto &[parents, currEdges]: connectedSetsBins) {
        // If there are more than 1 edges connecting two sets from
        // UnionFind, all of them are pseudo critical, because either
        // one can be chosen
        if (currEdges.size() > 1) {
          for (const auto &edge: currEdges) {
            pseudoCritical.insert(edge.index);
          }
        }

        // Add an edge to the graph
        //
        // We use the last edge for the index, because if there is only
        // one edge, this will give us its index and if there are more,
        // they are already marked pseudo critical above, so they won't
        // be affected by categorizing due to the contract
        // of the Graph.categorizeEdges method stating that
        // it does not add the edges that are in the second provided set
        // (in our case the pseudo critical edges set) to the first set
        // (in our case in the critical edges set), therefore
        // we don't care which index is used.
        graph.addEdge(
            {currEdges.back().index, parents.first, parents.second}
        );
        unionFind.unionSets(parents.first, parents.second);
      }

      graph.categorizeEdges(critical, pseudoCritical);
    }

    // Transform sets to vectors and return
    return {{critical.begin(),       critical.end()},
            {pseudoCritical.begin(), pseudoCritical.end()}};
  }
};