// https://leetcode.com/problems/interleaving-string/
//
// Given strings s1, s2, and s3, find whether s3 is formed
// by an interleaving of s1 and s2.
//
// An interleaving of two strings s and t is a configuration
// where s and t are divided into n and m
// non-empty substrings respectively, such that:
//  • s = s1 + s2 + ... + sn
//  • t = t1 + t2 + ... + tm
//  • |n - m| <= 1
//
// The interleaving is
// s1 + t1 + s2 + t2 + s3 + t3 + ... or
// t1 + s1 + t2 + s2 + t3 + s3 + ...
//
// Note: a + b is the concatenation of strings a and b.

class Solution {
public:
  static bool isInterleave(
      const string &s1, const string &s2, const string &s3
  ) {
    // Let's do a simple preliminary check to avoid problems with
    // indexing. Indeed, if the size of s3 doesn't equal the sum of
    // sizes of s1 and s2, then the answer is guaranteed to be false,
    // because according to the condition of the problem we have to use
    // all characters from s1 and s2 in the s3 construction

    if (s3.size() != s1.size() + s2.size()) {
      return false;
    }

    // The two lines below serve only for memory-usage optimization,
    // so for the sake of a clearer notation we assume (WLG) that
    // bigger is s1 and smaller is s2 in all the explanations below

    const string &bigger = s1.size() > s2.size() ? s1 : s2;
    const string &smaller = s1.size() > s2.size() ? s2 : s1;

    // Let's agree to the following notation:
    //   s[0...k] is a prefix of length k of s,
    //
    // [NOTE]: if k = 0 then s[0...k] is an empty string for any s
    //
    // [NOTE]: because we use zero-count everywhere, the m-th character
    //         of any string is at index m - 1,

    // We will make s1.size() iterations over this array,
    // after the i-th (in zero-count) iteration dp[j] will be
    // the correct result of this function for the following arguments:
    //
    //   s1 = s1[0...i]
    //   s2 = s2[0...j]
    //   s3 = s3[0...i + j]

    vector<bool> dp(smaller.size() + 1);

    // The update algorithm
    //
    // For the 0-th iteration
    //
    //   Because s1[0...0] is an empty string,
    //   dp[j] simply indicates if the prefixes of size j
    //   of s2 and s3 are equal, so:
    //
    //     j = 0: dp[0] = true,
    //            because an empty string equals an empty string
    //
    //     j ≠ 0: dp[j] = dp[j - 1] && smaller[j - 1] == s3[j - 1],
    //            because the prefixes of size j equal iff simultaneously:
    //              - the prefixes of size j - 1 equal
    //              - the last characters of the prefixes of size j equal
    //
    //
    // For the i-th iteration, where i ≠ 0
    //
    //   If dp[j] should be true, then s3[0...i + j] can be constructed
    //   from s1[0...i] and s2[0...j]. Let us consider the last
    //   character of s3[0...i + j]. It can be the last character
    //   of either s1[0...i] or s2[0...j].
    //
    //     In the first case the necessary condition of dp[j] being true
    //     is that s3[0...i + j - 1] can be constructed from s1[0...i - 1]
    //     and s2[0...j] (which, according to the induction hypothesis, is
    //     the value of dp[j] from the previous iteration)
    //
    //     In the second case the necessary condition of dp[j] being true
    //     is that s3[0...i + j - 1] can be constructed from s1[0...i] and
    //     s2[0...j - 1] (which, according to the induction hypothesis, is
    //     the value of dp[j - 1] from the current iteration)
    //
    //   Thus, the necessary condition of dp[j] being true is:
    //
    //     - either s3[i + j - 1] == s1[0...i - 1] and dp[j] (which we
    //       haven't yet updated from the previous iteration) is true
    //
    //     - or s3[i + j - 1] == s2[0...j - 1] and dp[j - 1] (which we
    //       have already updated on the current iteration) is true
    //
    //   Let's also notice that the conditions above also serves as
    //   the sufficient condition for dp[j] to be true:
    //
    //     The first case: if s3[0...i + j - 1] can be constructed from
    //     s1[0...i - 1] and s2[0...j] and s3[i + j - 1] == s1[i - 1],
    //     then we can construct s3[0...i + j] from s1[0...i] and s2[0...j]
    //     by taking the construction of s3[0...i + j - 1] and adding
    //     s1[i - 1] as the last piece
    //
    //     The second case is analogous
    //
    //   Therefore the provided condition is in fact both necessary and
    //   sufficient for dp[j] to be true, which concludes the proof
    //   of the algorithm

    dp[0] = true;
    for (size_t j = 1; j <= smaller.size(); j++) {
      dp[j] = dp[j - 1] && smaller[j - 1] == s3[j - 1];
    }

    for (size_t i = 1; i <= bigger.size(); i++) {
      for (size_t j = 0; j <= smaller.size(); j++) {
        if (j == 0) {
          dp[j] = dp[j] && bigger[i - 1] == s3[i - 1];
          continue;
        }

        dp[j] = (dp[j] && bigger[i - 1] == s3[i + j - 1]) ||
                (dp[j - 1] && smaller[j - 1] == s3[i + j - 1]);
      }
    }

    // After the last iteration the value below represents the correct
    // answer according to the definition of dp[j]

    return dp[smaller.size()];
  }
};