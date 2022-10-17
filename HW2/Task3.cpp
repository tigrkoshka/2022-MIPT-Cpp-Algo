// https://leetcode.com/problems/generate-parentheses/
//
// Given n pairs of parentheses, write a function
// to generate all combinations of well-formed parentheses.

class Solution {
private:
  static constexpr char open = '(';
  static constexpr char close = ')';

private:
  // Counts the n-th Catalan number
  // (see: https://en.wikipedia.org/wiki/Catalan_number)
  //
  // Takes up only O(n) operations on size_t, so this is quite fast
  static size_t countCombinations(size_t n) {
    // the answer is 1 for n == 0
    // (representing only an empty string)
    size_t nComb = 1;
    for (size_t i = 1; i <= n; ++i) {
      // the recursive formula
      nComb = 2 * (2 * i - 1) * nComb / (i + 1);
    }

    return nComb;
  }

  // Adds all combinations of well-formed parentheses
  // of length 2 * n (n opening and n closing parenthesis)
  // with prefix pref to ans
  //
  // [NOTE]: this function expects nOpen and nClose to be consistent with pref:
  //         - the former shows how many opening parenthesis are present in
  //         pref,
  //         - the latter shows how many closing parenthesis are present in pref
  //
  // [NOTE]: pref does not have to be a well-formed parenthesis combination
  static void addCombinationsWithPrefix(size_t n, string &pref, size_t nOpen,
                                        size_t nClose, vector<string> &ans) {
    // if the prefix has the desired length,
    // the only suitable combination is the pref itself,
    // so add it to ans and return
    if (pref.size() == 2 * n) {
      ans.push_back(pref);
      return;
    }

    // try adding an opening parenthesis to pref:
    // it is possible only if not all opening parenthesis
    // reserve is exhausted, i.e. if prefix contains less than n of them
    if (nOpen < n) {
      // add the opening parenthesis
      pref.push_back(open);
      // recurse with new prefix
      addCombinationsWithPrefix(n, pref, nOpen + 1, nClose, ans);
      // remove the added parenthesis, we don't need it anymore,
      // the prefix with it as the last symbol has been fully processed
      pref.pop_back();
    } else {
      // only closing parenthesis remain, add all of them at once and return
      // to avoid deepening the recursion unnecessarily
      string cl(n - nClose, close);
      ans.push_back(pref + cl);
      return;
    }

    // try adding a closing parenthesis to pref:
    // it only makes sense if the resulting prefix is in fact
    // a prefix of some well-formed combination of parentheses,
    // i.e. if the open-close parenthesis balance remains positive,
    // which is equivalent to the condition below
    if (nClose < nOpen) {
      // add the closing parenthesis
      pref.push_back(close);
      // recurse with new prefix
      addCombinationsWithPrefix(n, pref, nOpen, nClose + 1, ans);
      // remove the added parenthesis, we don't need it anymore,
      // the prefix with it as the last symbol has been fully processed
      pref.pop_back();
    }

    // at this point we expect to have entered at least one of the if-clauses
    // above, because if we haven't done so, that means that:
    //  - nOpen was n (otherwise we would have entered the first if-clause)
    //  - nClose was n (otherwise we would have entered the second if-clause)
    // which would lead pref to have the size of 2 * n, and we would not have
    // reached here due to the check at the beginning of this function
  }

public:
  static vector<string> generateParenthesis(size_t n) {
    vector<string> ans;
    // memory pre-allocation,
    // allocates O(4^n/n^1.5) elements (Catalan numbers asymptotic),
    // so the speed-up effect ought to be much more significant
    // than the slow-down caused by computing the capacity
    size_t nComb = countCombinations(n);
    ans.reserve(nComb);

    string pref;
    addCombinationsWithPrefix(n, pref, 0, 0, ans);

    // Uncomment this line to check countCombinations function
    // assert(ans.size() == nComb);

    return ans;
  }
};