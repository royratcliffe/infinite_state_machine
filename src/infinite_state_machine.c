/*
 * SPDX-FileCopyrightText: 2023, Roy Ratcliffe, Northumberland, United Kingdom
 * SPDX-License-Identifier: MIT
 */
/*!
 * \file infinite_state_machine.c
 * \brief Stack-based hierarchical (potentially unbounded logical) state machine implementation.
 *
 * Provides push and pop (enter and exit) semantics plus goto that performs
 * least–common–ancestor optimisation: only differing tail states are
 * exited or entered.
 *
 * Invariants:
 * - states[0..depth-1] are valid pointers, states[depth..] are \c NULL (after init or pop).
 * - depth <= INFINITE_STATE_MACHINE_MAX_DEPTH.
 *
 * Notes:
 * - Callbacks (enter and exit) are invoked after structural mutation so they observe the new stack.
 * - Callbacks must NOT directly corrupt depth or states; they may trigger further transitions
 *   only if higher-level code is designed for reentrancy.
 */

#include "infinite_state_machine.h"

#include <string.h>
#include <errno.h>

/*!
 * \brief Enters a state in the infinite state machine.
 * Only pushes a single state onto the stack and runs its enter actions.
 * \param machine The infinite state machine.
 * \param state The state to enter.
 * \return 0 on success, or a negative error code on failure.
 */
static int infinite_state_machine_enter(struct infinite_state_machine *machine, struct infinite_state *state);

/*!
 * \brief Exits a state in the infinite state machine.
 * Only pops a single state from the stack and runs its exit actions.
 * \param machine The infinite state machine.
 * \return 0 on success, or a negative error code on failure.
 */
static int infinite_state_machine_exit(struct infinite_state_machine *machine);

/*!
 * \brief Pushes a state onto the infinite state machine.
 * \param machine The infinite state machine.
 * \param state The state to push.
 * \return 0 on success, or a negative error code on failure.
 */
static int infinite_state_machine_push(struct infinite_state_machine *machine, struct infinite_state *state);

/*!
 * \brief Pops the top state from the infinite state machine.
 * \param machine The infinite state machine to pop from.
 * \return The popped state, or \c NULL if the machine is empty.
 */
static struct infinite_state *infinite_state_machine_pop(struct infinite_state_machine *machine);

void infinite_state_machine_init(struct infinite_state_machine *machine)
{
    machine->depth = 0;
    (void)memset(machine->states, 0, sizeof(machine->states));
}

void infinite_state_machine_goto(struct infinite_state_machine *machine, struct infinite_state *state)
{
    if (state == infinite_state_machine_top(machine))
    {
        return;
    }
    struct infinite_state_machine jump;
    infinite_state_machine_jump(&jump, state);
    int depth = 0;
    while (depth < machine->depth && depth < jump.depth && machine->states[depth] == jump.states[depth])
    {
        depth++;
    }
    while (machine->depth > depth)
    {
        infinite_state_machine_exit(machine);
    }
    while (jump.depth > depth)
    {
        infinite_state_machine_enter(machine, jump.states[depth++]);
    }
}

void infinite_state_machine_jump(struct infinite_state_machine *machine, struct infinite_state *state)
{
    infinite_state_machine_init(machine);
    machine->depth = infinite_state_topology(state, INFINITE_STATE_MACHINE_MAX_DEPTH, machine->states) - machine->states;
}

int infinite_state_machine_in(struct infinite_state_machine *machine, struct infinite_state *state)
{
    if (machine == NULL || state == NULL)
    {
        return -EINVAL;
    }
    for (int depth = 0; depth < machine->depth; depth++)
    {
        if (machine->states[depth] == state)
        {
            return 1;
        }
    }
    return 0;
}

int infinite_state_machine_enter(struct infinite_state_machine *machine, struct infinite_state *state)
{
    int err;
    if ((err = infinite_state_machine_push(machine, state)) < 0)
    {
        return err;
    }
    /*
     * Run the enter actions *after* the machine stack adds the state.
     * Technically, nothing prevents the action from applying yet another
     * state transition, although higher-level code must be designed for
     * reentrancy.
     */
    if (state->enter != NULL)
    {
        state->enter(state, machine);
    }
    return 0;
}

int infinite_state_machine_exit(struct infinite_state_machine *machine)
{
    struct infinite_state *state = infinite_state_machine_pop(machine);
    if (state == NULL)
    {
        return -EINVAL;
    }
    /*
     * Run the exit actions *after* the machine stack removes the state.
     */
    if (state->exit != NULL)
    {
        state->exit(state, machine);
    }
    return 0;
}

struct infinite_state *infinite_state_machine_top(const struct infinite_state_machine *machine)
{
    return machine->depth == 0 ? NULL : machine->states[machine->depth - 1];
}

int infinite_state_machine_push(struct infinite_state_machine *machine, struct infinite_state *state)
{
    if (machine->depth == INFINITE_STATE_MACHINE_MAX_DEPTH)
    {
        return -ENOMEM;
    }
    machine->states[machine->depth++] = state;
    return 0;
}

struct infinite_state *infinite_state_machine_pop(struct infinite_state_machine *machine)
{
    if (machine->depth == 0)
    {
        return NULL;
    }
    struct infinite_state *pop = machine->states[--machine->depth];
    /*
     * Clear the popped state from the stack. This is not strictly necessary,
     * but it can help catch use-after-free bugs.
     */
    machine->states[machine->depth] = NULL;
    return pop;
}
