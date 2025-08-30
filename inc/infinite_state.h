#ifndef INFINITE_STATE_H
#define INFINITE_STATE_H

/*
 * forward declaration of the infinite state machine
 */
struct infinite_state_machine;

/*!
 * \brief The state structure for the infinite state.
 * This structure represents a state in the infinite state machine. It contains
 * information about the state's parent, composite enter action, and composite
 * exit action.
 */
struct infinite_state
{
    /*!
     * \brief The parent state of this state.
     */
    struct infinite_state *super;

    /*!
     * \brief The enter action for this state.
     * \note This function is called when the state is entered in response to a
     * transition. The state is already pushed onto the machine stack when this
     * function is called. The function may be \c NULL, in which case no action
     * is taken.
     *
     * \param state The incoming sub-state.
     * \param machine The infinite state machine.
     */
    void (*enter)(struct infinite_state *state, struct infinite_state_machine *machine);

    /*!
     * \brief The exit action for this state.
     * \note This function is called when the state is exited in response to a
     * transition. The state is already popped from the machine stack \e before
     * this function is called. The function may be \c NULL, in which case no
     * action is taken.
     *
     * \param state The outgoing sub-state.
     * \param machine The infinite state machine.
     */
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
