#include <algorithm>
#include <cstdint>
#include <iostream>
#include <random>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

template<typename InputIter, typename OutputIter>
void myCopy(InputIter begin, InputIter end, OutputIter target) {
  for (auto iter{begin}; iter != end; ++iter, ++target) { *target = *iter; }
}

struct Person {
  std::string name;
  std::string surname;

  Person() = default; // to be able to generate on a pre-allocated memory

  Person(std::string name, std::string surname) : name(std::move(name)),
                                                  surname(std::move(surname)) {}
};

std::ostream &operator<<(std::ostream &out, const Person &person) {
  out << person.name << " " << person.surname;
  return out;
}

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
  const size_t nResult = 4;

  std::vector<PersonWithAge> peopleWithAge(nPeople);

  namespace ranges = std::ranges;
  namespace views = std::views;

  ranges::generate(peopleWithAge, generatePersonWithAge); // populate vector

  auto pipeline = views::all(peopleWithAge) |       // create view from vector
                  views::keys |                     // extract person
                  views::take(nResult);             // take the first 4 people

  ranges::copy(views::all(pipeline),
               std::ostream_iterator<Person>(std::cout, "\n"));

  auto extractSurname = [](const Person &p) { return p.surname; };

  ranges::copy(views::all(pipeline) |
               views::transform(extractSurname),
               std::ostream_iterator<std::string>(std::cout, "\n"));

  return 0;
}
