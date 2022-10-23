// https://leetcode.com/problems/unique-binary-search-trees-ii/
//
// Given an integer n, return all the structurally unique
// BST's (binary search trees), which has exactly n nodes of unique
// values from 1 to n. Return the answer in any order.

/*

// Uncomment this section for local testing

struct TreeNode {
  size_t val;
  TreeNode *left;
  TreeNode *right;

  TreeNode() : val(0),
               left(nullptr),
               right(nullptr) {}

  explicit TreeNode(size_t x) : val(x),
                                left(nullptr),
                                right(nullptr) {}

  TreeNode(size_t x, TreeNode *left, TreeNode *right) : val(x),
                                                        left(left),
                                                        right(right) {}
};

*/

class Solution {
private:
  struct PairHash {
    size_t operator()(const pair<uint32_t, uint32_t> &p) const {
      return (static_cast<size_t>(p.first) << 32ull) + p.second;
    }
  };

  // Semantics: {from, to} -> trees,
  // where trees are a vector of all BST's
  // consisting of nodes in range [from, to)
  using Cache =
      unordered_map<pair<uint32_t, uint32_t>, vector<TreeNode *>, PairHash>;

private:
  // Counts the Catalan numbers from 1 to n
  // (see: https://en.wikipedia.org/wiki/Catalan_number)
  //
  // Takes up only O(n) operations on size_t, so this is quite fast
  //
  // Proof:
  // Let the number of BST's in question for a given n be c_n.
  // Then c_0 = 1 (an empty tree)
  //      c_1 = 1 (a tree consisting of a single node)
  //      c_{n + 1} = \sum\limits_{i = 0}^n c_i * c_{n - i}, where:
  //         - i denotes the root of the tree
  //         - c_i is the number of options for the left subtree
  //         - c_{n - i} is the number of options for the right subtree,
  //           because there is the same number of BST's consisting of nodes
  //           with values from 1 to n - i as there is consisting of values
  //           from i + 1 to n (for a bijection subtract i from all nodes
  //           in the latter case)
  //         - the last two are multiplied, because they are independent
  // Therefore we get the recursive formula for the n-th Catalan number
  // (see the link above)
  static vector<size_t> countAllTrees(uint32_t n) {
    vector<size_t> nTrees(n + 1);

    // the answer is 1 for n == 0
    // (representing only an empty binary search tree)
    nTrees[0] = 1;
    for (uint32_t i = 1; i <= n; ++i) {
      // the recursive formula
      nTrees[i] = 2 * (2 * i - 1) * nTrees[i - 1] / (i + 1);
    }

    return nTrees;
  }

  // Performs a recursive deep copy of a tree,
  // used in the second commented section of generateTrees
  static TreeNode *copy(const TreeNode *sample) {
    if (sample == nullptr) {
      return nullptr;
    }

    return new TreeNode(sample->val, copy(sample->left), copy(sample->right));
  }

  // Recursively generates the BST's for nodes in range [from, to),
  // simultaneously filling the cache with subtrees
  //
  // Note that in the cache the trees aren't independent,
  // i.e. some pointers are used twice. That is ok, because we only
  // use the cache internally, and therefore are sure that we don't
  // touch the existing trees -- we only add new ones.
  // The generateTrees function contains a section
  // (now commented for the sake of speedup, but which should be uncommented
  // were this code to be used in a production-like environment)
  // which resolves the collisions by simply deep copying all the trees
  // in the final answer
  //
  // Returns a reference to definitely avoid copying (without relying on RVO)
  static vector<TreeNode *> &generateTreesRecurse(uint32_t from, uint32_t to,
                                                  Cache &cache) {
    // if we have already generated all subtrees for a range,
    // simply return the cached answer
    //
    // this serves as the breakpoint of the recursion,
    // because for the case 'from == to' the cache is
    // preliminarily filled in generateTrees
    vector<TreeNode *> &ans = cache[{from, to}];

    // this condition has the semantics 'is present in cache',
    // because a real answer cannot be an empty arrays,
    // because for all valid ranges at least one tree is generated
    // (in case 'from == to' the empty tree is put to cache in generateTrees)
    if (!ans.empty()) {
      // we don't make unnecessary copies of the trees,
      // see the comment to this function for explanation
      return ans;
    }

    // consequently, put all nodes to root and recurse
    // for the left and right subtrees to be generated
    // from the left and right sub-range respectively
    for (uint32_t root = from; root < to; ++root) {
      const auto &lefts = generateTreesRecurse(from, root, cache);
      const auto &rights = generateTreesRecurse(root + 1, to, cache);

      // the left and right subtrees are independent
      // (they consist of different node values),
      // so we have to record all possible combinations
      for (const auto &left : lefts) {
        for (const auto &right : rights) {
          // we don't make unnecessary copies of left and right,
          // see the comment to this function for explanation
          ans.push_back(new TreeNode(root, left, right));
        }
      }
    }

    return ans;
  }

public:
  static vector<TreeNode *> generateTrees(uint32_t n) {
    Cache cache;

    // memory pre-allocation,
    // allocates O(4^n/n^1.5) elements (Catalan numbers asymptotic)
    // for each n = to - from, where from \in [1, n], to \in [from + 1, n],
    // so the speed-up effect ought to be much more significant
    // than the slow-down caused by computing the capacities
    vector<size_t> nTrees = countAllTrees(n);
    for (uint32_t from = 1; from <= n; ++from) {
      for (uint32_t to = from + 1; to <= n + 1; ++to) {
        cache[{from, to}].reserve(nTrees[to - from]);
      }
    }

    // Fill corner cases: there is exactly one BST (an empty tree)
    // with from == to for any from value
    for (uint32_t from = 1; from <= n + 1; ++from) {
      cache[{from, from}].push_back(nullptr);
    }

    auto &ans = generateTreesRecurse(1, n + 1, cache);

    // The first section below is for debug
    // The second and the third ones make the code cleaner and more
    // suitable for production-like environment, but are commented now
    // for the sake of speedup

    /*

    // Uncomment this section to check countAllTrees function

    for (const auto &[key, trees]: cache) {
        assert(trees.size() == nTrees[key.second - key.first]);
    }

    */

    /*

    // Uncomment this section for a slower, but somewhat better solution,
    // which makes all the returned trees independent
    // (i.e. makes no pointers to be used twice)

    for (auto &tree: ans) {
        TreeNode *curr = tree;
        tree = copy(tree);

        delete curr;
    }

    */

    /*

    // Uncomment this section to avoid memory leaks

    for (const auto&[key, trees]: cache) {
        if (key.first == 1 && key.second == n + 1) {
            continue;
        }
        for (auto &tree: trees) {
            delete tree;
        }
    }

    */

    return ans;
  }
};