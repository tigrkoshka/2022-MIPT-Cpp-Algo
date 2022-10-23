// https://leetcode.com/problems/trapping-rain-water/
//
// Given n non-negative integers representing an elevation map where the
// width of each bar is 1, compute how much water it can trap after raining.

class Solution {
public:
  // This problem can be solved in various ways using O(1) memory and
  // O(heights.size()) time and those asymptotics can obviously not be
  // optimized. Therefore, the interesting part of the problem is making
  // non-asymptotic optimizations. The solution provided below is
  // quite optimized and may be not intuitive at first. So let's consider
  // the basic idea and a more intuitive algorithm before we prove
  // the provided one.

  // The general idea
  //
  // Let us denote the maximum value of heights as H.
  // Then let's denote the indices of the elements of heights which are
  // equal to H as i_1 ... i_k in ascending order.
  // Finally, we denote i_1 as L and i_k as R.
  //
  // Now let's notice that the amount of water that can be trapped
  // at positions L and R is 0, because there is no higher bar to hold
  // that water per the definitions of L and R.
  //
  // Next, we deduce that the amount of water that can be trapped between
  // L and R equals to all the empty volume (not filled with bars)
  // between L and R, i.e. if L == R that is zero, else that is:
  //    (R - L - 1) * H - \sum\limits_{i = L + 1}^{R - 1} heights[i],
  //
  // Finally, we notice that the water that can be trapped at index
  // i < L is equal to lMax[i] - heights[i], where lMax is the maximum
  // height from indices 0 to i inclusively, because the water to
  // the left of L can only leak to the left, because to the right it is
  // bounded by the bar at index L, which is strictly higher than
  // all the bars to the left by the definition of L.
  //
  // Similarly, the water that can be trapped at index i > L is equal to
  // rMax[i] - heights[i], where rMax is the maximum height from indices
  // i to heights.size() - 1 inclusively.

  // Implementations:

  // 1. Brute force: do exactly as the idea suggests

  static size_t trapBruteForce(const vector<size_t> &heights) {
    size_t H = 0;
    size_t L = 0, R = 0;

    for (size_t i = 0; i < heights.size(); ++i) {
      if (heights[i] < H) {
        continue;
      }

      if (heights[i] == H) {
        R = i;
        continue;
      }

      H = heights[i];
      L = i;
      R = i;
    }

    size_t volume = 0;

    size_t lMax = 0;
    for (size_t i = 0; i < L; ++i) {
      if (lMax > heights[i]) {
        volume += lMax - heights[i];
      } else {
        lMax = heights[i];
      }
    }

    // this can be slightly optimized by first computing
    // the sum height between L and R and then using the formula
    // from the 'General idea' section, but it pollutes the code
    // and that is a minor optimization to the idea-demonstrating code,
    // so we skip it
    for (size_t i = L + 1; i < R; ++i) {
      volume += H - heights[i];
    }

    size_t rMax = 0;
    for (size_t i = heights.size() - 1; i > R; --i) {
      if (rMax > heights[i]) {
        volume += rMax - heights[i];
      } else {
        rMax = heights[i];
      }
    }

    return volume;
  }

  // Complexity
  //
  //   Let's denote heights.size() as n.
  //   We consider the separate worst cases for each category:
  //     - 3n + (n - 1)      = 4n - 1     assignments (including +=)
  //     - 3n + 2(n - 1) + 2 = 5n         comparisons
  //     - n + 3(n - 1)      = 4n - 1     arithmetic operations

  // 2. Let's prove the provided algorithm. We use two pointers
  //    (left and right) and remember the current lMax and rMax.
  //    Each branch of the outer if-condition is the same as either
  //    the second or the fourth for-loop in the first implementation.
  //    Because on each iteration we move exactly one pointer,
  //    at some point we will reach either L or R. Let's (WLG) consider
  //    that L was reached first. Then after that lMax never changes
  //    (because it is already equal to H), and only the right pointer
  //    is moved until it reaches R. So, for the parts to the left of L
  //    and to the right of R the implementation below computes the same
  //    answer as the first one. After L and R have been reached,
  //    regardless of the branch of the outer condition we get in,
  //    we will never get inside the else-clause of the inner if,
  //    because both lMax and rMax are equal to H at that point.
  //    So, it some fashion (depending on the heights values)
  //    we simply traverse the iterations of the third for-loop
  //    from the implementation above. Therefore, we add all the same values
  //    to volume, thus getting the same answer as the implementation above,
  //    which was proved to be correct.

  // The only reason the vector of heights has int template parameter
  // instead of the more semantically correct size_t is because
  // otherwise Leetcode cannot compile this
  static size_t trap(const vector<int> &heights) {
    size_t volume = 0;

    size_t left = 0, right = heights.size() - 1;
    size_t lMax = 0, rMax = 0;

    while (left < right) {
      if (heights[left] < heights[right]) {
        if (lMax > heights[left]) {
          volume += lMax - heights[left];
        } else {
          lMax = heights[left];
        }

        ++left;
      } else {
        if (rMax > heights[right]) {
          volume += rMax - heights[right];
        } else {
          rMax = heights[right];
        }

        --right;
      }
    }

    return volume;
  }

  // Complexity
  //
  //   Once again, let's denote heights.size() as n:
  //     - (always)    n      assignments (including +=)
  //     - (always)   3n      comparisons
  //     - (at most)  2n      arithmetic operations

  // As we can see, the second approach is about
  // twice as fast as the first one
};