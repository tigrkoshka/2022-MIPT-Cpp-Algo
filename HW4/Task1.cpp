#include <algorithm>
#include <cstdint>
#include <iostream>
#include <ranges>
#include <string>

int main() {
  const uint32_t from = 10, to = 100;

  auto square = [](uint32_t num) { return num * num; };
  auto isNotMul5 = [](uint32_t num) { return num % 5 != 0; };
  auto to_string = [](uint32_t num) { return std::to_string(num); };
  auto print = [](const std::string &str) { std::cout << str << std::endl; };

  namespace views = std::views;

  auto pipeline = views::iota(from, to) |    // generate sequence [from, to)
                  views::transform(square) | // square the resulting numbers
                  views::filter(isNotMul5);  // filter out multiples of 5

  std::ranges::for_each(
      pipeline,
      print,    // print the resulting numbers to stdout...
      to_string // ...after converting them to string
  );

  return 0;
}