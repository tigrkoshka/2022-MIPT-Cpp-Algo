// https://leetcode.com/problems/single-number-iii/
//
// Given an integer array nums, in which exactly two elements appear only once
// and all the other elements appear exactly twice. Find the two elements that
// appear only once. You can return the answer in any order.
//
// You must write an algorithm that runs in linear runtime complexity
// and uses only constant extra space.

class Solution {
public:
    vector<int> singleNumber(vector<int>& nums) {
        // Phase 1
        //
        // In this phase we obtain a bitwise xor of the two target values.
        // To do that we just apply a bitwise xor reduction to all the elements
        // of the given array. Indeed, considering that the xor operation is
        // commutative and for any Y the expression 'Y xor Y' evaluates to 0,
        // all the other elements (which appear exactly twice per the condition
        // of the problem) will cancel each other out, leaving us with
        // the desired result.

        long long xorOfTwo = 0;
        for (auto num : nums) {
            xorOfTwo ^= num;
        }

        // Phase 2
        //
        // In this phase we get a value in which only one bit is set to 1: that
        // is the lowest of the differing bits of the two target values. Such
        // a bit exists because the two target values differ as per
        // the condition of the problem.
        //
        // To do that we use a trick using the xor value obtained in
        // the previous phase. Indeed, let's consider the said xor value
        // (ignoring the sign bit). It has those and only those bits set to 1
        // which differ in the two target values. Its additive inverse (again,
        // ignoring the sign bit), therefore, has those and only those bits set
        // to 0 which were set to 1 in the value itself, excluding the lowest of
        // such bits. The sign bit will, of course, be the inverse of the sign
        // bit of the initial xor value. Therefore, if we apply a bitwise and
        // operation to the xor value and its additive inverse, the resulting
        // value will be exactly the target for this phase.
        //
        // For more detailed explanation on how negative integers
        // are represented, please refer to:
        // https://en.wikipedia.org/wiki/Two%27s_complement

        xorOfTwo &= -xorOfTwo;

        // Phase 3
        //
        // Now we can introduce two bins for the values on the array:
        // those which have the bit found in the previous phase set to 1 and
        // those which have it set to 0. Each bin contains exactly one of
        // the target values as per the definition of said bit. All the other
        // values fall into bins in pairs of two equal values. Therefore, if we
        // now apply the bitwise xor reduction to the values in each of the bins
        // we will get the target values.
        //
        // For performance reasons, we do not form the bins explicitly, instead
        // we just create an array of two values representing the bins and
        // iterate over the initial array updating those values using a bitwise
        // xor operation each time a value from the initial array falls into
        // the respective bin.

        vector<int> res(2);
        for (auto num : nums) {
            res[(num & xorOfTwo) == 0] ^= num;
        }

        return res;
    }
};
