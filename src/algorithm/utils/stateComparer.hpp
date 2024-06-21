#pragma once

#include "../../formulas/formula.hpp"
#include "state.hpp"
#include "successorTree.hpp"
#include "successorTreeComparison.hpp"
#include <memory>
#include <vector>

template <typename T>
concept SharedPtrOrRawPtrToState = std::is_same_v<T, std::shared_ptr<state>> || std::is_same_v<T, state *>;

template <typename C>
concept ContainerOfStates = requires(C c) {
  {
    c.begin()
  } -> std::same_as<typename C::iterator>;
  {
    c.end()
  } -> std::same_as<typename C::iterator>;
  {
    c.empty()
  } -> std::same_as<bool>;
  requires SharedPtrOrRawPtrToState<typename C::value_type>;
};

/// @brief This class is responsible for comparing states and trees of states using a bunch of static methods
class StateComparer {
public:
  /// @brief Given two vector of states, this function returns true if the two vectors contain the same states. The order of the
  /// states does not matter.
  /// @param s1 - The first vector of states
  /// @param s2 - The second vector of states
  /// @return True if the two vectors contain the same states, false otherwise
  template <ContainerOfStates StateContainer>
  static bool sameStates(const StateContainer & states1, const StateContainer & states2, bool considerInternalVariables)
  {
    auto result1 = std::all_of(states1.begin(), states1.end(), [&states2, &considerInternalVariables](const auto & state1) {
      return containState(states2, state1.get(), considerInternalVariables);
    });
    auto result2 = std::all_of(states2.begin(), states2.end(), [&states1, &considerInternalVariables](const auto & state2) {
      return containState(states1, state2.get(), considerInternalVariables);
    });
    return result1 && result2;
  }

  /// @brief Given a state and a list of states, this function returns true if the given state is in the list of states.
  /// @param states - The list of states to compare with
  /// @param s - The state to compare with
  /// @return True if the given state is in the list of states, false otherwise
  template <ContainerOfStates StateContainer>
  static bool containState(const StateContainer & states, const state * s, bool considerInternalVariables)
  {
    return std::any_of(states.begin(), states.end(), [&s, &considerInternalVariables](const auto & state) {
      return state->isSame(s, considerInternalVariables);
    });
  }

  /// @brief Given a state and a list of states, this function returns the state that is most similar to the given state.
  /// @param current - The state to compare with
  /// @param states - The list of states to compare with
  /// @return The state that is most similar to the given state
  template <ContainerOfStates StateContainer, SharedPtrOrRawPtrToState StatePtr>
  static StatePtr most_similar_state(const StatePtr current, const StateContainer & states)
  {
    bool considerInternalVariables = true;
    return minMaxState(states, current, considerInternalVariables).first;
  }

  template <ContainerOfStates StateContainer, SharedPtrOrRawPtrToState StatePtr>
  static StatePtr most_different_state(const StatePtr current, const StateContainer & states)
  {
    bool considerInternalVariables = false;
    return minMaxState(states, current, considerInternalVariables).second;
  }

  /// @brief Given two lists of states, this function returns the states that are in the first list but not in the second list.
  /// @param states_original - The original list of states
  /// @param states_mutant - The list of states to compare with
  /// @return A list of states that are in the first list but not in the second list
  template <ContainerOfStates StateContainer>
  static StateContainer distinct_states(const StateContainer & states_original, const StateContainer & states_mutant)
  {
    StateContainer distinct;
    bool considerInternalVariables = false;
    for (const auto & original_state : states_original) {
      bool found = containState(states_mutant, original_state, considerInternalVariables);
      if (!found) {
        distinct.push_back(original_state);
      }
    }
    return distinct;
  }

  static successorTreeComparison compareKSuccessors(const successorTree & original_successors,
                                                    const successorTree & mutant_successors);

private:
  template <ContainerOfStates StateContainer, SharedPtrOrRawPtrToState StatePtr>
  static std::pair<StatePtr, StatePtr> minMaxState(const StateContainer & states, const StatePtr & state,
                                              bool considerInternalVariables)
  {
    auto it_pair = std::minmax_element(states.begin(), states.end(),
                                      [&state, &considerInternalVariables](const auto & s1, const auto & s2) {
                                        return s1->delta(state, considerInternalVariables) < s2->delta(state, considerInternalVariables);
                                      });
    return {*it_pair.first, *it_pair.second};
  }
};