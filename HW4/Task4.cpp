#include <algorithm>
#include <cstdint>
#include <iostream>
#include <ranges>
#include <vector>

namespace ranges = std::ranges;
namespace views = std::views;

std::vector<int32_t> readWithFor() {
  std::vector<int32_t> numbers;

  for (int32_t num: views::istream<int32_t>(std::cin)) {
    if (num == -1) {
      break;
    }

    numbers.push_back(num);
  }

  return numbers;
}

std::vector<int32_t> readWithoutFor() {
  const int32_t eofNum = -1;

  auto notEOFNum = [](int32_t num) { return num != eofNum; };

  auto pipeline = views::istream<int32_t>(std::cin) |
                  views::take_while(notEOFNum);

  // We do need to have an explicit for-loop of some kind (in this case ranges::for_each),
  // because none of the following approaches work:
  //
  //   - First idea: just copy the elements
  //
  //     ranges::copy(pipeline, numbers.begin())
  //
  //     We haven't allocated the memory yet, so this would result in UB and most probably a crash
  //
  //   - Second idea: let's try allocating the necessary memory in the previous approach
  //
  //     std::vector<int32_t> numbers(ranges::distance(pipeline));
  //     ranges::copy(pipeline, numbers.begin())
  //
  //     Since views are lazy and do not store any elements, the pipeline above
  //     reads from cin until it meets eofNum EVERY time it is iterated through,
  //     and we use two iterations here (one in ranges::distance and one in ranges::copy),
  //     so the pipeline would expect us to enter new numbers
  //
  //   - Third idea: let's use the vector constructor accepting two iterators:
  //
  //     std::vector<int32_t> numbers(pipeline.begin(), pipeline.end());
  //
  //     This would result in a compile-time error since the iterators cannot be converted to std::vector<int32_t>::iterator
  //
  //   - Fourth idea: let's try using views::common in the previous approach to make the iterators convertible
  //
  //     auto commonPipeline = pipeline | views::common;
  //     std::vector<int32_t> numbers(commonPipeline.begin(), commonPipeline.end());
  //
  //     This also results in a compile-time error, because even though this would work with other types of views,
  //     the iterators of views::istream are non-copyable, only movable, and the views::common, like the vector constructor,
  //     cannot work in such cases, because it manages the C++17 iterators, which are required to be copyable
  //
  // In the C++23 standard there is a specific adaptor exactly for this use-case.
  // When it is released and supported by the compiler, we will be able to write:
  //
  //   std::vector<int32_t> numbers = pipeline | ranges::to<std::vector>()

  std::vector<int32_t> numbers;

  auto pushNumber = [&numbers](int32_t num) { numbers.push_back(num); };

  ranges::for_each(pipeline, pushNumber);

  return numbers;
}

int main() {
  auto print = [](int32_t value) { std::cout << value << ' '; };

  // auto numbers = readWithFor();
  auto numbers = readWithoutFor();

  ranges::for_each(numbers, print);

  return 0;
}
