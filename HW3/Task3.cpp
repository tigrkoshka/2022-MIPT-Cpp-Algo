// https://leetcode.com/problems/coin-change/
//
// You are given an integer array coins representing coins of
// different denominations and an integer amount representing
// a total amount of money.
//
// Return the fewest number of coins that you need to make up that amount.
//
// If that amount of money cannot be made up
// by any combination of the coins, return -1.
//
// You may assume that you have an infinite number of each kind of coin.

class Solution {
public:
  // The only reason the vector of coins has int template parameter
  // instead of the more semantically correct uint64_t is because
  // otherwise Leetcode cannot compile this
  static ssize_t coinChange(const vector<int> &coins, uint64_t amount) {
    vector<uint64_t> sortedCoins(coins.begin(), coins.end());
    sort(sortedCoins.begin(), sortedCoins.end());

    // We will be filling this array element-by-element making coins.size()
    // iterations over each element. We proceed to the next element
    // only after the current one has been fully processed. After the i-th
    // element was fully processed, dp[i] will be the correct result
    // of this function for the following arguments:
    //
    //   coins = coins
    //   amount = i

    vector<ssize_t> dp(amount + 1);

    // The update algorithm
    //
    // For the 0-th element the correct answer is 0, because we can make up
    // a 0 amount by using exactly 0 coins and the answer cannot get less
    // than that
    //
    //
    // For the i-th element, where i ≠ 0
    //
    //   The amount of i cannot be made up by using 0 coins, so at least
    //   one coin was used.
    //
    //   The coins[k] coin could have been used iff simultaneously:
    //     - coins[k] is not greater than i
    //     - the amount of i - coins[k] can be made up using the given coins
    //
    //   If those conditions are met then the minimal total number of
    //   coins needed to make up i using at least one coin with coins[k]
    //   denomination is one more than the correct answer of this function
    //   for the same coins and amount = i - coins[k] (because if we use
    //   the coins[k] coin, we just have to minimize the number of coins
    //   used to make up the i - coins[k] amount, which is exactly what
    //   this function returns). Note that when calculating the value for
    //   dp[i] we already know the correct answer of this function
    //   with the mentioned arguments: it is stored in dp[i - coins[k]]
    //   according to the induction hypothesis.
    //
    //   Therefore, the correct value to write to dp[i] is one more than
    //   the minimum of dp[i - coins[k]] for those values of k which
    //   satisfy the two conditions above. That concludes the proof
    //   of the algorithm.

    // NOTES:
    //
    //   - The algorithm is optimized for the large majority of cases
    //     by sorting the coins array beforehand. That allows us to
    //     stop iterating through coins if the first condition from the two
    //     mentioned above is not met at some iteration. Therefore, for all
    //     amounts in range [coins[k], coins[k + 1]) we only make k
    //     iterations instead of coins.size(). That is usually a significant
    //     speedup (compared to the slow-down cause by sorting), because
    //     the coins array is usually small compared to the denominations
    //     of the coins.
    //
    //     Note, that unlike the initial code on Leetcode we accept
    //     a constant vector of coins because that is more semantically
    //     correct for this function, but compels us to copy the vector
    //     before sorting it. However, that does not diminish
    //     the optimization effect, because it only takes up O(coins.size())
    //     operations when the total cost of the optimization is
    //     O(coins.size() * log(coins.size())).
    //
    //   - The algorithm can be slightly further optimized by setting
    //     dp[i] to something like SSIZE_MAX instead of -1, which would
    //     allow us to skip one of the conditions in the inner for-loop.
    //     However, that optimization is very slight and pollutes the code,
    //     so it has been omitted.

    dp[0] = 0;

    for (uint64_t currAmount = 1; currAmount <= amount; ++currAmount) {
      dp[currAmount] = -1;

      for (uint64_t coin : sortedCoins) {
        if (currAmount < coin) {
          break;
        }

        if (dp[currAmount - coin] == -1) {
          continue;
        }

        if (dp[currAmount] == -1) {
          dp[currAmount] = dp[currAmount - coin] + 1;
          continue;
        }

        dp[currAmount] = min(dp[currAmount], dp[currAmount - coin] + 1);
      }
    }

    return dp[amount];
  }
};