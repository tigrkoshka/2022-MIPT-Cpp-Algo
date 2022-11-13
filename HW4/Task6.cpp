#include <type_traits>
#include <cstdint>
#include <iostream>
#include <string>

template<typename T>
concept Awesome =
    requires(T a) { std::to_string(a); } &&
    std::is_arithmetic<T>::value &&
    !std::is_same<T, bool>::value;

template<Awesome T>
std::string concat(T first, T second) {
  return std::to_string(first) + std::to_string(second);
}

int main() {
  int64_t i1 = 2, i2 = 39;
  std::cout << concat(i1, i2) << std::endl;

  uint32_t ui1 = 2, ui2 = 39;
  std::cout << concat(ui1, ui2) << std::endl;

  float f1 = 2.39, f2 = 4.56;
  std::cout << concat(f1, f2) << std::endl;

  double c = 2.39, d = 4.56;
  std::cout << concat(c, d) << std::endl;

  // CE: the required expression 'std::__cxx11::to_string(a)' is invalid
  // std::cout << concat(&a, &b) << std::endl;

  // CE: the expression '!(std::is_same<T, bool>::value) [with T = bool]' evaluated to 'false'
  // std::cout << concat(true, false) << std::endl;
}
