/*
 * SPDX-FileCopyrightText: 2023, Roy Ratcliffe, Northumberland, United Kingdom
 * SPDX-License-Identifier: MIT
 */

/*!
 * \file infinite_state.c
 * \brief Implementation of infinite state machine topology traversal.
 *
 * The \c infinite_state_topology() function performs a depth-limited upward
 * traversal of the state hierarchy, collecting unique super-states (including
 * the starting state) into the supplied topology array. The function returns a
 * pointer to the next free slot after the last written state. Duplicate
 * suppression (to avoid loops) is compiled in only when \c DEBUG is defined.
 *
 * Preconditions:
 *  - depth >= 0; when depth == 0 no states are added.
 *  - topology points to storage for at most depth states.
 *
 * Postconditions:
 *  - Returned pointer equals topology + n where 0 <= n <= depth.
 *
 * Notes:
 *  - If cycles are possible in non-debug builds, consider unconditional
 *    duplicate detection or a visited flag to guarantee safety.
 *  - Could \c assert(depth>=0) for defensive programming.
 */
#include "infinite_state.h"

#include <stddef.h>

struct infinite_state **infinite_state_topology(struct infinite_state *state, int depth,
                                                struct infinite_state **topology)
{
    if (state == NULL || depth <= 0)
    {
        return topology;
    }

    struct infinite_state *chain[depth];
    int count = 0;

    for (struct infinite_state *current = state; current != NULL && count < depth;
         current = current->super)
    {
#ifdef DEBUG
        for (int index = 0; index < count; index++)
        {
            if (chain[index] == current)
            {
                current = NULL;
                break;
            }
        }
#endif
        if (current == NULL)
        {
            break;
        }
        chain[count++] = current;
    }

    for (int index = count; index > 0; index--)
    {
        *topology++ = chain[index - 1];
    }

    return topology;
}
