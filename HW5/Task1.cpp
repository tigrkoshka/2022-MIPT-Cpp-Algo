// https://leetcode.com/problems/minimum-weighted-subgraph-with-the-required-paths/
//
// You are given an integer n denoting the number of nodes of
// a weighted directed graph. The nodes are numbered from 0 to n - 1.
//
// You are also given a 2D integer array edges where
// edges[i] = [from_i, to_i, weight_i] denotes that there exists
// a directed edge from from_i to to_i with weight weight_i.
//
// Lastly, you are given three distinct integers src1, src2, and dest
// denoting three distinct nodes of the graph.
//
// Return the minimum weight of a subgraph of the graph such that it is
// possible to reach dest from both src1 and src2 via a set of edges
// of this subgraph. In case such a subgraph does not exist, return -1.
//
// A subgraph is a graph whose vertices and edges are subsets
// of the original graph. The weight of a subgraph is the sum
// of weights of its constituent edges.

class Solution {
private:
  struct Edge {
    uint64_t from;
    uint64_t to;
    uint64_t weight;

    explicit Edge(const vector<int> &edge) : from(edge[0]),
                                             to(edge[1]),
                                             weight(edge[2]) {}

    Edge(uint64_t from_, uint64_t to_, uint64_t weight_) : from(from_),
                                                           to(to_),
                                                           weight(weight_) {}

    [[nodiscard]] Edge revert() const {
      return {to, from, weight};
    }
  };

  using Edges = vector<vector<int>>;
  // AdjacencyList[i] represents all edges from the i-th vertex
  using AdjacencyLists = vector<vector<Edge>>;

private:
  // Converts an edges list representation to
  // an adjacency lists representation of a graph
  static AdjacencyLists edgesToAdjacencyLists(size_t n, const Edges &edges) {
    AdjacencyLists adjLists(n);

    for (const auto &edge: edges) {
      adjLists[edge[0]].emplace_back(edge);
    }

    return adjLists;
  }

  // Returns the adjacency lists for the graph,
  // which is the reverse of the one represented
  // by the specified adjacency lists
  static AdjacencyLists revert(const AdjacencyLists &adjLists) {
    AdjacencyLists reverse(adjLists.size());

    for (const auto &list: adjLists) {
      for (const auto &edge: list) {
        reverse[edge.to].push_back(edge.revert());
      }
    }

    return reverse;
  }

  // Computes the minimum distances from the src-th vertex to all the rest
  // in the graph represented by the specified adjacency lists using
  // a Dijkstra algorithm, a clear detailed explanation of which can be
  // found, for example, here:
  // https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm
  static vector<uint64_t> minimumDistances(const AdjacencyLists &adjLists,
                                           uint64_t src) {
    // Semantics:
    //
    // distances[i] is the minimal found at the current iteration distance
    // from the src-th vertex to the i-th one or UINT64_MAX if the i-th
    // vertex is unreachable from the src-th one
    //
    // Initialization:
    //
    // In the beginning we cannot determine the distances to any vertices
    // other than the src-th itself (the distance to which is, obviously, 0)
    // or even if those vertices are reachable from the src-th, so we
    // initialize all the elements except for the src-th as UINT64_MAX
    vector<uint64_t> distances(adjLists.size(), UINT64_MAX);
    distances[src] = 0;

    // Semantics:
    //
    // The queue contains pairs [distance_to_i, i], so that it is sorted by
    // the distances and not the vertex labels
    //
    // Template:
    //
    // We need the descending order,
    // so we specify all three template parameters
    priority_queue<pair<uint64_t, uint64_t>,
                   vector<pair<uint64_t, uint64_t>>,
                   greater<>> queue;
    queue.emplace(distances[src], src);

    while (!queue.empty()) {
      // Extract a vertex from the top of the queue, i.e. the one,
      // which is determined as the closest to the src-th vertex at
      // the current iteration
      uint64_t from = queue.top().second;
      uint64_t extractedDist = queue.top().first;
      queue.pop();

      // This check is a technical one, our algorithm for the sake
      // of performance allows a vertex to be recorded in the queue
      // multiple times, all but one of which represent deprecated states
      // from other iterations; only the non-deprecated one will fail
      // at this check
      if (extractedDist != distances[from]) {
        continue;
      }

      // Update the distances to the vertices adjacent
      // to the extracted one: we can reach them by visiting
      // the extracted vertex and then going along the edge
      // connecting it to the vertex in question,
      // so if the previously determined distance is bigger
      // than the one we get following this route,
      // it is not the optimal one
      for (auto edge: adjLists[from]) {
        uint64_t to = edge.to;
        if (distances[to] > distances[from] + edge.weight) {
          distances[to] = distances[from] + edge.weight;
          queue.emplace(distances[to], to);
        }
      }
    }

    return distances;
  }

public:
  // The only reason the vector of edges has int template parameter
  // instead of the more semantically correct uint64_t is because
  // otherwise Leetcode cannot compile this.
  //
  // Idea:
  //
  // The paths from src1 and src2 to dst are guaranteed to have a common
  // vertex (dst is always the common vertex of these paths).
  //
  // Let us consider then the first one of such common vertices in the order
  // of our visits to them when following the mentioned paths. Let's denote
  // it as X.
  //
  // The minimum weight of a subgraph containing such paths from src1 and src2
  // to dst that X is a common vertex for them is the graph consisting of:
  //
  //   - the shortest path from src1 to X,
  //   - the shortest path from src2 to X,
  //   - the shortest path from X to dst.
  //
  // That is because any subgraph containing the described paths from
  // src1 to dst and from src2 to dst obviously contains paths from src1 to X,
  // from src2 to X and from X to dst. These paths are independent in that
  // choosing one of them does not affect the choice of the rest. So, if we
  // choose the shortest for each we will have an overall minimum
  // of the resulting subgraph's weight.
  //
  // Algorithm:
  //
  // The algorithm iterates through all vertices of the specified graph,
  // finds the minimum weights of the subgraphs described above and
  // then returns the minimum of those weights as the answer.
  //
  // Implementation:
  //
  // Since we are iterating through all vertices trying them as the X vertex,
  // we are going to need the minimum distances from src1 and src2 to all
  // vertices and from all vertices to dst. We precalculate those distances
  // using the Dijkstra algorithm. After that, we simply use the found
  // minimum distances to compute the overall minimum weights of the subgraphs
  // described above as the dum of the weights of the three paths mentioned
  // in the 'Idea' section
  static uint64_t minimumWeight(
      size_t n, const Edges &edges,
      uint64_t src1, uint64_t src2, uint64_t dst
  ) {
    AdjacencyLists adjLists = edgesToAdjacencyLists(n, edges);

    vector<uint64_t> distFromSrc1 = minimumDistances(adjLists, src1);
    vector<uint64_t> distFromSrc2 = minimumDistances(adjLists, src2);
    vector<uint64_t> distToDst = minimumDistances(revert(adjLists), dst);

    uint64_t minWeight = UINT64_MAX;
    for (size_t i = 0; i < n; ++i) {
      // avoid overflow
      if (distFromSrc1[i] == UINT64_MAX ||
          distFromSrc2[i] == UINT64_MAX ||
          distToDst[i] == UINT64_MAX) {
        continue;
      }

      minWeight = min(minWeight, distFromSrc1[i] +
                                     distFromSrc2[i] +
                                     distToDst[i]);
    }

    return minWeight == UINT64_MAX ? -1 : minWeight;
  }
};