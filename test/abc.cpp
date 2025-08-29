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

static infinite::state_machine<my_state> ism;

ostream &operator<<(ostream &os, const my_state *s) {
  if (s)
    os << s->name;
  else
    os << "nullptr";
  return os;
}

ostream &operator<<(ostream &os,
                    const infinite::state_machine<my_state>::gone &gone) {
  os << "Exited states: ";
  for (const auto &state : gone.exits) {
    os << state->self()->name << " ";
  }
  os << endl;
  os << "\nEntered states: ";
  for (const auto &state : gone.entries) {
    os << state->self()->name << " ";
  }
  os << endl;
  return os;
}

extern "C" int test_abc() {
  auto done = ism.go(&c);
  cout << done;
  assert(ism.at() == &c);
  assert(ism.in(&a));
  assert(ism.in(&b));
  cout << ism.go(&a);
  return 0;
}
