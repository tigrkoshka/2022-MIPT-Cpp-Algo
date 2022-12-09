// https://leetcode.com/problems/best-time-to-buy-and-sell-stock/
//
// You are given an array prices where prices[i] is the price of a given
// stock on the i-th day.
//
// You want to maximize your profit by choosing a single day to buy one
// stock and choosing a different day in the future to sell that stock.
//
// Return the maximum profit you can achieve from this transaction.
// If you cannot achieve any profit, return 0.

class Solution {
public:
    // The brute force approach to this problem is to take all pairs (i, j) such
    // that j ≥ i and get the maximum of the differences on the stock prices
    // on the j-th and on the i-th day. However, that algorithm takes up O(n^2)
    // time.
    //
    // To optimize that algorithm, let's notice that if the price on the i-th
    // day is greater than the price on the k-th day, where i < k, then our
    // resulting maximum profit cannot be obtained by buying the stock on
    // the i-th day and selling it on the j-th day, where j > k. Indeed, if we
    // bought the stock on the k-th day instead of the i-th and sold it on
    // the same j-th day, our profit would have been greater.
    //
    // Therefore, in the approach above we only need to consider such pairs
    // (i, j) that j ≥ i and for all k such that i < k < j the price on the k-th
    // day is greater than the price on the i-th day. Or, which is equivalent
    // we can get the maximum of the differences between the price on the i-th
    // day and the minimum price on days before the i-th.
    //
    // To do that we maintain an additional variable representing the minimum
    // price on prefix before the current value and on each step we calculate
    // the profit as the difference between the current stock price and
    // the minimum on the prefix before it.
    static int maxProfit(vector<int>& prices) {
        int minPrice = INT_MAX;
        int maxProfit = 0;

        for (auto price : prices) {
            minPrice = min(minPrice, price);
            maxProfit = max(maxProfit, price - minPrice);
        }

        return maxProfit;
    }
};
