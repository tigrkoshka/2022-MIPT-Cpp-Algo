#include <stdio.h>

class Prism {
private:
  double height;

protected:
  explicit Prism(double height_) : height(height_) {}
  virtual ~Prism() = default;

public:
  virtual double Square() const = 0;

  double Volume() const { return height * Square(); }
};

class Box : public Prism {
private:
  double side;

public:
  Box(double height_, double side_) : Prism(height_), side(side_) {}

  double Square() const { return side * side; }
};

class Cube : public Box {
public:
  explicit Cube(double side_) : Box(side_, side_) {}
};

int main() {
  const Prism *p, *q, *r;
  Box a(0.5, 2), b(5, 0.2);
  Cube c(0.5);
  p = &a;
  q = &b;
  r = &c;
  printf("Squares: %3.3lf %3.3lf %3.3lf\n", p->Square(), q->Square(),
         r->Square());
  printf("Volumes: %3.3lf %3.3lf %3.3lf\n", p->Volume(), q->Volume(),
         r->Volume());
  return 0;
}