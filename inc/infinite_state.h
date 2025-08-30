#ifndef INFINITE_STATE_H
#define INFINITE_STATE_H

/*
 * forward declaration of the infinite state machine
 */
struct infinite_state_machine;

struct infinite_state
{
    struct infinite_state *super;
    void (*enter)(struct infinite_state *state, struct infinite_state_machine *machine);
    void (*exit)(struct infinite_state *state, struct infinite_state_machine *machine);
};

/*!
 * \brief Get the topology of the infinite state machine.
 * \details Compute a state's forward topological vector.
 * This function fills the topology vector with the state's sub-states
 * in "forward" order (from the outer state down to the inner).
 * The function returns a pointer to the end of the filled vector.
 * \param state The current state.
 * \param depth The current depth.
 * \param topology The topology array to fill.
 * \return The updated topology array.
 */
struct infinite_state **infinite_state_topology(struct infinite_state *state, int depth,
                                                struct infinite_state **topology);

#endif /* INFINITE_STATE_H */
