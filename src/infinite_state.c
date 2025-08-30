#include "infinite_state.h"

#include <stddef.h>

struct infinite_state **infinite_state_topology(struct infinite_state *state, int depth,
                                                struct infinite_state **topology)
{
    /*
     * Get the topology of the infinite state machine.
     * This function traverses the state hierarchy and collects all states
     * in the specified topology array.
     */
    if (state == NULL || depth == 0)
    {
        return topology;
    }
    struct infinite_state **sub = infinite_state_topology(state->super, depth - 1, topology);
#ifdef DEBUG
    for (struct infinite_state **super = topology; super != sub; super++)
    {
        if (*super == state)
        {
            // If we found the state in the super-states, we need to stop.
            return sub;
        }
    }
#endif
    *sub = state;
    return sub + 1;
}
