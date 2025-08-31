/*
 * SPDX-FileCopyrightText: 2023, Roy Ratcliffe, Northumberland, United Kingdom
 * SPDX-License-Identifier: MIT
 */
/*!
 * \file infinite_state_machine.h
 * \brief Public API for the infinite state machine.
 * Provides functions to initialise, perform hierarchical transitions (goto and
 * jump), query whether a state is active, and get the current top state.
 *
 * The machine keeps a stack (array) of active states up to
 * \c{INFINITE_STATE_MACHINE_MAX_DEPTH}. Not thread-safe; external synchronisation
 * is required for concurrent use.
 */

#ifndef INFINITE_STATE_MACHINE_H
#define INFINITE_STATE_MACHINE_H

#include "infinite_state.h"

/*!
 * \brief Maximum depth of any infinite state machine.
 * Defaults to 7 unless already defined before the inclusion point of this header.
 *
 * Why 7? There is method in the choice a maximum depth of 7. Pointers and
 * integers are 32 bits wide on a 32-bit machine. Seven pointers and one integer
 * occupy 8 words, or 32 bytes. If a single machine requires more than seven
 * levels of nesting, better to refactor the design to reduce complexity.
 */
#ifndef INFINITE_STATE_MACHINE_MAX_DEPTH
#define INFINITE_STATE_MACHINE_MAX_DEPTH 7
#endif

/*!
 * \brief Represents an infinite state machine.
 * This structure holds the current state hierarchy and allows for transitions
 * between states.
 * \note The structure is not thread-safe.
 */
struct infinite_state_machine
{
    /*!
     * \brief The states in the infinite state machine.
     */
    struct infinite_state *states[INFINITE_STATE_MACHINE_MAX_DEPTH];

    /*!
     * \brief The current depth of the infinite state machine.
     */
    int depth;
};

/*!
 * \brief Initialises the infinite state machine.
 * The machine is reset to its initial state. It has an initial depth of 0.
 * All its states are cleared to \c NULL for safety.
 * \param machine The infinite state machine to initialise.
 */
void infinite_state_machine_init(struct infinite_state_machine *machine);

/*!
 * \brief Goes to a state in the infinite state machine.
 * \param machine The infinite state machine.
 * \param state The state to enter.
 *
 * Going to a state in the infinite state machine transitions the machine to the
 * new state. This will push the current state onto the stack and transition to
 * the new state. If the new state is the same as the current state, no action
 * is taken; likewise, if the new state is \c{NULL}, no action is taken.
 * Otherwise, all the exit actions for the current state are run, and the new
 * state is entered by running all enter actions.
 *
 * \note O(n) time complexity applies, where n is the depth of the state machine.
 */
void infinite_state_machine_goto(struct infinite_state_machine *machine, struct infinite_state *state);

/*!
 * \brief Jumps to a state in the infinite state machine.
 * \param machine The infinite state machine.
 * \param state The state to jump to.
 *
 * Jumping to a state in the infinite state machine resets the machine to its
 * initial state and then transitions to the specified state. This is useful
 * for resetting the machine to a known state without going through the
 * normal entry and exit actions.
 *
 * \note O(n) time complexity applies, where n is the depth of the state machine.
 */
void infinite_state_machine_jump(struct infinite_state_machine *machine, struct infinite_state *state);

/*!
 * \brief Checks if a state is currently active in the infinite state machine.
 * \param machine The infinite state machine.
 * \param state The state to check.
 * \return 1 if the state is active, 0 if it is not, or a negative error code on failure.
 */
int infinite_state_machine_in(struct infinite_state_machine *machine, struct infinite_state *state);

/*!
 * \brief Gets the top state of the infinite state machine.
 * \param machine The infinite state machine.
 * \return The top state, or \c NULL if the machine is empty.
 */
struct infinite_state *infinite_state_machine_top(const struct infinite_state_machine *machine);

#endif /* INFINITE_STATE_MACHINE_H */
