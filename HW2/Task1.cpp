// https://leetcode.com/problems/permutations-ii/
//
// Given a collection of numbers, nums, that might contain duplicates,
// return all possible unique permutations in any order.

class Solution {
private:
  // Counts n! recursively
  static size_t factorial(size_t n) {
    if (n == 0) {
      return 1;
    }

    return n * factorial(n - 1);
  }

  // Counts the respective multinomial coefficient
  // (see: https://en.wikipedia.org/wiki/Multinomial_theorem#Multinomial_coefficients)
  //
  // Takes up only O(n) operations on size_t, so this is quite fast
  // Proof:
  //  It computes n! and the factorials of the sizes of all bins.
  //  Since n! computation complexity is O(n), and the sum of the sizes of all
  //  bins is n, it ultimately performs O(2n) = O(n) operations
  static size_t countPermutations(size_t fullSize,
                                  const unordered_map<int, size_t> &bins) {
    size_t nPermutations = factorial(fullSize);
    for (auto [_, nOccurrences] : bins) {
      nPermutations /= factorial(nOccurrences);
    }

    return nPermutations;
  }

  // Computes the bins of elements,
  // i.e. a mapping element -> number of its occurrences in nums
  static unordered_map<int, size_t> countElements(const vector<int> &nums) {
    unordered_map<int, size_t> bins;
    for (int elem : nums) {
      bins[elem]++;
    }

    return bins;
  }

  // Adds all permutations of elements in bins with prefix pref to ans
  //
  // [NOTE]: this function expects fullSize and pref to be consistent with bins:
  //         the former must be equal to the sum of bin sizes in the latter
  //         plus the size of the pref
  static void addPermutationsWithPrefix(size_t fullSize,
                                        unordered_map<int, size_t> &bins,
                                        vector<int> &pref,
                                        vector<vector<int>> &ans) {
    // if the prefix contains all the elements,
    // the only suitable permutation is the prefix itself,
    // so add it to ans and return
    if (pref.size() == fullSize) {
      ans.push_back(pref);
      return;
    }

    // try adding new elements to the prefix
    for (auto [elem, nOccurrences] : bins) {
      // if the element has 0 occurrences left,
      // we cannot add it to the prefix, so skip it
      if (nOccurrences == 0) {
        continue;
      }

      // add the element to the prefix
      pref.push_back(elem);
      // register that one occurrence of the element was used
      --bins[elem];
      // recurse with the new prefix
      addPermutationsWithPrefix(fullSize, bins, pref, ans);
      // remove the added element, we don't need it anymore,
      // the prefix with it as the last element has been fully processed
      pref.pop_back();
      // register that the occurrence of the element is free to be reused
      ++bins[elem];
    }

    // at this point we expect to have iterated at least once through the
    // for-loop above, because if we haven't done so, that means that all
    // the sizes of bins are zeros, which would mean that the size of
    // the pref is equal to fullSize due to the contract of this function,
    // and we would not have reached here due to the check
    // at the beginning of this function
  }

public:
  static vector<vector<int>> permuteUnique(const vector<int> &nums) {
    // get bins of elements
    unordered_map<int, size_t> bins = countElements(nums);

    vector<vector<int>> ans;
    // memory pre-allocation,
    // allocates O(n!) elements (the worst case),
    // so the speed-up effect ought to be much more significant
    // than the slow-down caused by computing the capacity
    size_t nPermutations = countPermutations(nums.size(), bins);
    ans.reserve(nPermutations);

    vector<int> pref;
    addPermutationsWithPrefix(nums.size(), bins, pref, ans);

    // Uncomment this line to check countPermutations function
    // assert(ans.size() == nPermutations);

    return ans;
  }
};