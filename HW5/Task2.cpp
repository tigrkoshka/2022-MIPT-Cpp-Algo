// https://leetcode.com/problems/min-cost-to-connect-all-points/
//
// You are given an array points representing integer coordinates
// of some points on a 2D-plane, where points[i] = [x_i, y_i].
//
// The cost of connecting two points [x_i, y_i] and [x_j, y_j] is
// the manhattan distance between them: |x_i - x_j| + |y_i - y_j|,
// where |val| denotes the absolute value of val.
//
// Return the minimum cost to make all points connected.
// All points are connected if there is exactly one
// simple path between any two points.

class Solution {
private:
  // Computes the manhattan distance between two points
  static uint64_t manhattan(const vector<int> &first,
                            const vector<int> &second) {
    return abs(first[0] - second[0]) +
           abs(first[1] - second[1]);
  }

public:
  // This is basically a problem of finding the weight of
  // the minimum spanning tree in a full graph, in which the vertices
  // are represented by the points and the weights of the edges are defined
  // as the manhattan distances between said points.
  //
  // We use the Prim's algorithm, a clear detailed explanation of which
  // can be found, for example, here:
  // https://en.wikipedia.org/wiki/Prim%27s_algorithm
  static uint64_t minCostConnectPoints(const vector<vector<int>> &points) {
    uint64_t totalWeight = 0;

    // Semantics:
    //
    // While the i-th vertex is not yet added to the resulting graph,
    // distances[i] contains the minimum distance from the i-th vertex
    // to a vertex, which is already added to the resulting graph.
    //
    // After the i-th vertex is added to the resulting graph
    // distances[i] is UINT64_MAX.
    //
    // We initialize distances with UINT32_MAX to denote that the distances
    // were not yet defined and not to confuse with visited vertices,
    // for which UINT64_MAX is recorded, as explained above.
    vector<uint64_t> distances(points.size(), UINT32_MAX);

    // The vertex which was latest to be added to the resulting graph.
    // Initially, that is the 0-th (which is just a random choice,
    // any point could be the first one added to the resulting graph).
    uint64_t latest = 0;

    for (size_t connected = 1; connected < points.size(); ++connected) {
      // Mark the latest added vertex as added.
      distances[latest] = UINT64_MAX;

      // Update the distances taking into account the latest added vertex
      // and search for the next vertex to add, which is the vertex with
      // the minimum resulting distance to the already assembled part
      // of the resulting graph.
      //
      // We initialize it with points.size() + 1 meaning that the next
      // point has not yet been chosen.
      uint64_t next = latest;
      for (uint64_t curr = 1; curr < points.size(); ++curr) {
        // Do not consider already added vertices.
        if (distances[curr] == UINT64_MAX) {
          continue;
        }

        // Update the distance from the curr-th vertex to the already
        // assembled part of the resulting graph taking into account
        // the latest added vertex.
        distances[curr] = min(distances[curr],
                              manhattan(points[latest], points[curr]));

        // Update the vertex we are going to visit
        // next as described above
        if (distances[curr] < distances[next]) {
          next = curr;
        }
      }

      totalWeight += distances[next];
      latest = next;
    }

    return totalWeight;
  }
};