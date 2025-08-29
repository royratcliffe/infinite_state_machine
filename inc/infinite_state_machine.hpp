// SPDX-License-Identifier: MIT
//! \file infinite_state_machine.hpp
//! \details This file contains the definition of the infinite state machine,
//! allowing for complex state hierarchies and transitions.

#ifndef INFINITE_STATE_MACHINE_HPP_
#define INFINITE_STATE_MACHINE_HPP_

// for efficient double-ended queue operations
#include <deque>

// for find algorithms
#include <algorithm>

// Why infinite state machine? This state machine allows for an arbitrary number
// of nested states, enabling complex state hierarchies and transitions. No
// limits on the nesting depth, i.e. the number of active states is only limited
// by the available memory.
namespace infinite {

//! \brief A default abstract state representation.
//! \details This struct serves as a base for all states in the state machine,
//! unless otherwise overridden by a derived class.
template <typename Topology> struct state {
  using state_ptr = Topology *;
  state_ptr super;
  state_ptr self() { return static_cast<state_ptr>(this); }
};

//! \brief A state machine topology navigation class.
//! \details This class provides methods to navigate through the state machine's
//! topology, allowing for transitions between states and querying the current
//! state.
template <typename Topology> class state_machine {
public:
  //! \brief Destructor for the state machine.
  //! \details Cleans up the state machine and releases any resources.
  virtual ~state_machine() {}

  // state_machine (const state_machine &other);
  // state_machine (state_machine &&other);
  // state_machine& operator= (state_machine &&other);
  // state_machine& operator= (const state_machine &other);

  //! \brief A struct representing the states exited and entered during a
  //! transition.
  //! \details This struct holds the states that were exited and entered during
  //! a transition, allowing for easy access to this information after a
  //! transition completes. They appear in the correct order. Run all the exit
  //! handlers for the exited states from back to front, then run all the entry
  //! handlers for the entered states similarly.
  struct gone {
    std::deque<state<Topology> *> exits, entries;
  };

  //! \brief Transition to a new state.
  //! \details This function handles the state transition logic, including the
  //! management of entry and exit states.
  //!
  //! Do not make any assumptions about the topology. Specifically, do not
  //! assume that one state does not occupy more than one state machine terrain.
  //! If a state \e does appear in overlapping topologies, then the state will
  //! appear in both the exits \e and entries. Consider this a correct
  //! conclusion since the duplicated state re-enters from a different
  //! super-state. The nesting differs.
  //!
  //! \param to The new state to transition to.
  //! \return A struct containing the states that were exited and entered during
  //! the transition.
  struct gone go(state<Topology> *to) {
    std::deque<state<Topology> *> exits, entries;
    // super, sub --> sub, super (reverse)
    while (!states.empty()) {
      exits.push_front(states.back());
      states.pop_back();
    }
    if (to != nullptr) {
      // super.super, super, to!
      //
      // Avoid duplicating states in the entries deque.
      // Duplicates correspond to cyclic state topologies.
      for (; to && std::find(entries.cbegin(), entries.cend(), to) == entries.cend();
           to = to->super)
        entries.push_front(to);
      // Now match up the exits and entries.
      // Transfer any matching states from exits to states.
      while (!exits.empty() && !entries.empty() &&
             exits.front() == entries.front()) {
        states.push_back(exits.front());
        exits.pop_front();
        entries.pop_front();
      }
      for (auto state : entries)
        states.push_back(state);
      // or states.push_back(entries);
    }
    std::reverse(exits.begin(), exits.end());
    return {exits, entries};
  }

  // Operation go(to) is the only mutator.
  // The rest are query methods on the state vector.

  //! \brief Get the current state.
  //! \return The current state, or \c nullptr if there is no active state.
  state<Topology> *at() const {
    return states.empty() ? nullptr : states.back();
  }

  //! \brief Check if a state is active.
  //! \param state The state to check.
  //! \return Answers \c true if the state is active, \c false otherwise.
  bool in(state<Topology> *state) const {
    return std::find(states.cbegin(), states.cend(), state) != states.cend();
  }

private:
  //! \brief The deque holding the active states.
  //! \details This deque maintains the order of active states, allowing for
  //! efficient nested state transitions and queries.
  //!
  //! The front of the deque represents the outermost state, while the back
  //! represents the innermost (current) state.
  //!
  //! \note This deque is not thread-safe and should be accessed
  //! only from a single thread.
  std::deque<state<Topology> *> states;
};

} /* namespace infinite */

#endif /* INFINITE_STATE_MACHINE_HPP_ */
