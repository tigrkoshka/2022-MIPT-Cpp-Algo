// https://leetcode.com/problems/redundant-connection-ii/
//
// In this problem, a rooted tree is a directed graph such that,
// there is exactly one node (the root) for which all other nodes
// are descendants of this node, plus every node has exactly one parent,
// except for the root node which has no parents.
//
// The given input is a directed graph that started as a rooted tree
// with n nodes (with distinct values from 1 to n), with one additional
// directed edge added. The added edge has two different vertices chosen
// from 1 to n, and was not an edge that already existed.
//
// The resulting graph is given as a 2D-array of edges. Each element of edges
// is a pair [ui, vi] that represents a directed edge connecting
// nodes ui and vi, where ui is a parent of child vi.
//
// Return an edge that can be removed so that the resulting graph
// is a rooted tree of n nodes. If there are multiple answers,
// return the answer that occurs last in the given 2D-array.

class Solution {
public:
  static vector<int> findRedundantDirectedConnection(vector<vector<int>> &edges) {
    //------------------------------Phase 1---------------------------------

    // In this phase we try to find a vertex that has two parents,
    // in which case the answer is one of the respective edges

    // Semantics: parent[i] is the parent of the i-th edge that we have last
    // encountered. Because edges contain the edges of a valid tree
    // plus one extra edge, it can be guaranteed that no more than one
    // collision can occur, and we are looking specifically for that
    // collision (if it exists). So if by the time we want to write
    // j to parent[i], it already has k (≠ 0, because the vertices are
    // 1-indexed) stored in it, then the collision is
    // that i has two parents: j and k.
    //
    // Since edges contain all edges of a valid tree
    // plus one extra edge, the size of edges equals the number of vertices,
    // but since vertices are 1-indexed, unlike the vector, we create
    // a vector of a size that is 1 bigger than the edges size and consider
    // the 0-th element invalid
    vector<int> parent(edges.size() + 1);

    // The edges connecting the two parents of the vertex
    // in question. So, in the above example candidates will be
    // the {j, i} and the {k, i} edges.
    //
    // For one of them we store itself, for another we store its index
    // in the edges vector. That type of storage is simply an optimization
    // for the following checks and does not affect the semantics of
    // the algorithm
    //
    // If we don't found a collision, the former will be an empty vector
    // and the latter will be SIZE_T_MAX (which we denote as -1 only because
    // Leetcode cannot compile SIZE_T_MAX and UINT64_MAX would be incorrect
    // considering that size_t is not always the same as uint64_t)
    size_t candidateForExtra = -1;
    vector<int> anotherCandidateForExtra;

    for (size_t i = 0; i < edges.size(); ++i) {
      int from = edges[i][0], &to = edges[i][1];

      // If we have not yet encountered a parent for the to-th vertex,
      // set the parent to from and continue our search
      if (parent[to] == 0) {
        parent[to] = from;
        continue;
      }

      // If parent[to] is not 0, then it represents an already
      // encountered parent of the to-th vertex, so the candidates
      // are either the previously encountered edge or the current one

      // We know the index for one of the candidates, because it is the
      // current edge, so we store that index
      candidateForExtra = i;
      anotherCandidateForExtra = {parent[to], to};

      // We have found what we have been looking for, no need to continue
      break;
    }

    //------------------------------Phase 2---------------------------------

    // In this phase we look at our tree without the candidateForExtra
    // edge (if such was found). If the tree is valid, the candidateForExtra
    // is our answer. Otherwise, if a collision did occur in the previous
    // phase, another candidate is the answer. If no collision occurred,
    // then the extra edge creates a cycle and an edge from that cycle
    // id the answer

    // We reinitialize the parents vector considering each vertex
    // to form its own tree (consisting only of itself)
    for (size_t i = 1; i <= edges.size(); i++) {
      parent[i] = i;
    }

    for (size_t i = 0; i < edges.size(); ++i) {
      // Skip our candidate for being the extra edge
      if (i == candidateForExtra) {
        continue;
      }

      int from = edges[i][0], to = edges[i][1];

      // Find the root of the from-th vertex using the relations
      // we have so far encountered
      int rootOfFrom = root(parent, from);

      // Cycle occurred
      if (rootOfFrom == to) {
        // If we didn't find a collision in the first phase, then any
        // edge of the found cycle can be considered the extra edge.
        // According to the task we have to return the last of them
        // in order of their placement in edges, which is the current
        // one, because we detect a cycle only when we have seen all the
        // edges that form it
        if (anotherCandidateForExtra.empty()) {
          return edges[i];
        }

        // If we ded find a collision in the first phase, tried
        // removing one of the candidates and still got an invalid tree,
        // then the extra edge is another candidate, because in this
        // case only the two candidates can possibly be the extra edge
        return anotherCandidateForExtra;
      }

      // Add the to-th vertex to the same tree that the from-th is in
      parent[to] = rootOfFrom;
    }

    // If the tree became OK without our candidate, then the candidate
    // is indeed the extra edge, so we return it
    return edges[candidateForExtra];
  }

private:
  // Finds the root of a tree containing the vertex according to the
  // parents relations
  static int root(vector<int> &parent, int vertex) {
    if (parent[vertex] != vertex) {
      parent[vertex] = root(parent, parent[vertex]);
    }

    return parent[vertex];
  }
};