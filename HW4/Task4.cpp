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

  std::vector<int32_t> numbers;
  ranges::copy(pipeline, std::back_inserter(numbers));

  return numbers;
}

int main() {
  auto print = [](int32_t value) { std::cout << value << ' '; };

  // auto numbers = readWithFor();
  auto numbers = readWithoutFor();

  ranges::for_each(numbers, print);

  return 0;
}
