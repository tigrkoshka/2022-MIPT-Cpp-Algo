// https://leetcode.com/problems/subsets-ii/
//
// Given an integer array nums that may contain duplicates,
// return all possible subsets (the power set).
//
// The solution set must not contain duplicate subsets.
// Return the solution in any order.

class Solution {
private:
  // Counts the total number of subsets
  //
  // Explanation:
  //  Assume we have k unique elements {a_1, ..., a_k},
  //  which occur in nums {m_1, ..., m_k} times respectively.
  //  Then to for a subset we simply have to choose a number
  //  of occurrences of each unique element in that subject,
  //  which for a_i is equal to m_i + 1 (we can take it from 0 to m_i times
  //  inclusively), so the total number of subsets, according to the rule of
  //  product, is (m_1 + 1) * ... * (m_k + 1)
  //
  // Takes up only O(n) operations on size_t, so this is quite fast
  //
  // [NOTE]: this function expects nums to be sorted
  static size_t countSubsets(const vector<int> &nums) {
    // the answer is 1 for empty array
    // (representing only an empty subset)
    size_t nSubsets = 1;
    for (size_t i = 0; i < nums.size(); ++i) {
      // the number of repetitions for the current element
      size_t nCurrRepetitions = 1;
      // move forward while the element value remains the same
      for (; i < nums.size() - 1 && nums[i] == nums[i + 1]; ++i) {
        ++nCurrRepetitions;
      }

      // see explanation above
      nSubsets *= (nCurrRepetitions + 1);
    }

    return nSubsets;
  }

  // Adds all subsets, which:
  //  - have pref as a subset
  //  - may differ from pref only by elements of num from index analyze_from
  // to ans
  //
  // [NOTE]: this function expects nums to be sorted
  static void addSubsetsWithPrefix(const vector<int> &nums, vector<int> &pref,
                            size_t analyze_from, vector<vector<int>> &ans) {
    // pref satisfies the conditions from the contract of this function,
    // so we add it to ans
    ans.push_back(pref);

    // try adding elements to pref in the following fashion:
    //  - we only add one element directly
    //  - we recurse for more additions
    for (size_t i = analyze_from; i < nums.size(); ++i) {
      // if we have already encountered the current element,
      // that means we have already processed all possibilities for
      // pref appended with the current elements, so skip
      if (i > analyze_from && nums[i] == nums[i - 1]) {
        continue;
      }

      // add the current element
      pref.push_back(nums[i]);
      // recurse with the new prefix,
      // using the remaining elements of nums (analyze_from = i + 1)
      addSubsetsWithPrefix(nums, pref, i + 1, ans);
      // remove the appended element, we don't need it anymore,
      // the prefix with it as the last element has been fully processed
      pref.pop_back();
    }
  }

public:
  static vector<vector<int>> subsetsWithDup(vector<int> &nums) {
    // this is quite a bad code style, because the function name
    // implies that nums would not be changed: something is simply generated
    // from it, so in production it would probably accept nums by const
    // reference, copy it and sort the copy, but since this is a leetcode task,
    // we will allow such behaviour to avoid space and time complexity increase
    sort(nums.begin(), nums.end());

    vector<vector<int>> ans;
    // memory pre-allocation,
    // allocates O(2^n) elements (the worst case),
    // so the speed-up effect ought to usually be much more significant
    // than the slow-down caused by computing the capacity
    size_t nSubsets = countSubsets(nums);
    ans.reserve(nSubsets);

    vector<int> pref;
    addSubsetsWithPrefix(nums, pref, 0, ans);

    // Uncomment this line to check countSubsets function
    // assert(ans.size() == nSubsets);

    return ans;
  }
};