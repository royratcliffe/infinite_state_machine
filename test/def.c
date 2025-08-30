#include "infinite_state_machine.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

static void enter_action(struct infinite_state *state,
                         struct infinite_state_machine *machine);
static void exit_action(struct infinite_state *state,
                        struct infinite_state_machine *machine);

struct state {
  struct infinite_state infinite;
  const char *name;
};

static struct state d = {.name = "d",
                         .infinite.super = NULL,
                         .infinite.enter = enter_action,
                         .infinite.exit = exit_action};
static struct state e = {.name = "e",
                         .infinite.super = &d.infinite,
                         .infinite.enter = enter_action,
                         .infinite.exit = exit_action};
static struct state f = {.name = "f",
                         .infinite.super = &e.infinite,
                         .infinite.enter = enter_action,
                         .infinite.exit = exit_action};
static struct state g = {.name = "g",
                         .infinite.super = &d.infinite,
                         .infinite.enter = enter_action,
                         .infinite.exit = exit_action};

static struct infinite_state_machine ism;

int test_def() {
  infinite_state_machine_init(&ism);
  infinite_state_machine_goto(&ism, &f.infinite);
  assert(infinite_state_machine_in(&ism, &d.infinite) == 1);
  assert(infinite_state_machine_in(&ism, &e.infinite) == 1);
  assert(infinite_state_machine_in(&ism, &f.infinite) == 1);
  assert(infinite_state_machine_in(&ism, &g.infinite) == 0);
  infinite_state_machine_goto(&ism, &g.infinite);
  assert(infinite_state_machine_in(&ism, &d.infinite) == 1);
  assert(infinite_state_machine_in(&ism, &e.infinite) == 0);
  assert(infinite_state_machine_in(&ism, &f.infinite) == 0);
  assert(infinite_state_machine_in(&ism, &g.infinite) == 1);
  return 0;
}

static void enter_action(struct infinite_state *state,
                         struct infinite_state_machine *machine) {
  struct state *s = (struct state *)state;
  assert(s->name != NULL);
  printf("enter %s\n", s->name);
}

static void exit_action(struct infinite_state *state,
                        struct infinite_state_machine *machine) {
  struct state *s = (struct state *)state;
  assert(s->name != NULL);
  printf("exit %s\n", s->name);
}
