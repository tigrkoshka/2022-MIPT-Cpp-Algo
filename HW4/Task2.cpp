#include <algorithm>
#include <cstdint>
#include <iostream>
#include <random>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

template<typename InputIter>
auto sum(InputIter begin, InputIter end) {
  auto sum{*begin};

  for (auto iter{++begin}; iter != end; ++iter) {
    sum += *iter;
  }

  return sum;
}

struct Person {
  std::string name;
  std::string surname;

  Person() = default; // to be able to generate on a pre-allocated memory

  Person(std::string name, std::string surname)
      : name(std::move(name)), surname(std::move(surname)) {}
};

typedef std::pair<Person, uint32_t> PersonWithAge;

PersonWithAge generatePersonWithAge() {
  static const std::vector<std::string> names = {
      "Marie", "Nicole", "Sam", "John", "Ivan", "Tanya", "Maxim", "Anna",
      "Michel", "Kate", "Thomas", "Garry", "Bob", "Taylor", "Emily",
  };

  static const std::vector<std::string> surnames = {
      "Black", "Smith", "Doe", "Cadeau", "Brown", "Atkins", "Campbell",
      "Walker", "Green", "Wilson", "Daniels", "Donovan", "Evans", "Fisher",
      "Young",
  };

  static std::random_device device;
  static std::mt19937 engine{device()};

  static std::uniform_int_distribution<size_t> name_distribution{0, names.size() - 1};
  static std::uniform_int_distribution<size_t> surname_distribution{0, surnames.size() - 1};
  static std::uniform_int_distribution<uint32_t> age_distribution{1, 80};

  Person person(names[name_distribution(engine)],
                surnames[surname_distribution(engine)]);
  uint32_t age = age_distribution(engine);

  return {person, age};
}

int main() {
  const size_t nPeople = 100;
  const uint32_t minAge = 12, maxAge = 65;

  std::vector<PersonWithAge> peopleWithAge(nPeople);

  namespace ranges = std::ranges;
  namespace views = std::views;

  ranges::generate(peopleWithAge, generatePersonWithAge); // populate people

  auto extractAge = [](const PersonWithAge &p) { return p.second; };
  auto filterAge = [](uint32_t age) { return age >= minAge && age <= maxAge; };

  auto pipeline = views::all(peopleWithAge) |    // create view from vector
                  views::transform(extractAge) | // extract age from pairs
                  views::filter(filterAge);      // filter out too young and too old

  std::cout << static_cast<double_t>(sum(pipeline.begin(), pipeline.end())) /
                   ranges::distance(pipeline);

  return 0;
}
