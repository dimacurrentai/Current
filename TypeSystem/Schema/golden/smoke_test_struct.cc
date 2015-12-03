// g++ -c -std=c++11 current.cc

#include "current.h"

namespace current_userspace {
struct Primitives {
  uint8_t a;
  uint16_t b;
  uint32_t c;
  uint64_t d;
  int8_t e;
  int16_t f;
  int32_t g;
  int64_t h;
  char i;
  std::string j;
  float k;
  double l;
  bool m;
  std::chrono::microseconds n;
  std::chrono::milliseconds o;
};
struct A {
  int32_t a;
};
struct B : A {
  int32_t b;
};
struct Empty {
};
struct X {
  int32_t x;
};
struct Y {
  int32_t x;
};
struct C {
  Empty e;
  OptionalPolymorphic<X, Y> c;
};
struct FullTest {
  Primitives primitives;
  std::vector<std::string> v1;
  std::vector<Primitives> v2;
  std::pair<std::string, Primitives> p;
  Optional<Primitives> o;
  Polymorphic<A, B, C, Empty> q;
  OptionalPolymorphic<A, B, C> r;
};
}  // namespace current_userspace