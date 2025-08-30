#include "infinite_state_machine.hpp"

#include <cassert>
#include <iostream>

using namespace std;

struct my_state : infinite::state<my_state> {
  const char *name;
};

static my_state a = {nullptr, "a"};
static my_state b = {&a, "b"};
static my_state c = {&b, "c"};
static my_state d = {&a, "d"};

static infinite::state_machine<my_state> ism;

ostream &operator<<(ostream &os, const my_state *s) {
  if (s)
    os << s->name;
  else
    os << "nullptr";
  return os;
}

ostream &
operator<<(ostream &os,
           const infinite::state_machine<my_state>::transition &transition) {
  os << "exited (";
  for (const auto &state : transition.exits) {
    os << " " << state->self()->name;
  }
  os << " ) entered (";
  for (const auto &state : transition.enters) {
    os << " " << state->self()->name;
  }
  os << " )";
  return os;
}

extern "C" int test_abc() {
  cout << ism.go(&c) << endl;
  assert(ism.at() == &c);
  assert(ism.in(&a));
  assert(ism.in(&b));
  auto transition = ism.go(&a);
  cout << transition << endl;
  assert(transition.enters.empty());
  assert(transition.exits.size() == 2);
  assert(transition.exits[0] == &c);
  assert(transition.exits[1] == &b);
  assert(ism.at() == &a);
  assert(ism.in(&a));
  assert(!ism.in(&b));
  assert(!ism.in(&c));
  cout << "transition to c: " << ism.go(&c) << endl;
  cout << "transition to d: " << ism.go(&d) << endl;
  return 0;
}
