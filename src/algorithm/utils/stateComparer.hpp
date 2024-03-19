#pragma once

#include "../../formulas/formula.hpp"
#include "state.hpp"
#include "successorTree.hpp"
#include "successorTreeComparison.hpp"
#include <memory>
#include <vector>

/// @brief This class is responsible for comparing states and trees of states using a bunch of static methods
class StateComparer {
public:
  static bool sameStates(const std::vector<std::shared_ptr<state>> & states1,
                         const std::vector<std::shared_ptr<state>> & states2, bool considerInternalVariables);

  static bool containsFormula(const std::vector<std::shared_ptr<formula>> & formulas, const std::shared_ptr<formula> & formula);

  static std::vector<std::shared_ptr<formula>> removeDuplicates(const std::vector<std::shared_ptr<formula>> & formulas);

  /// @brief Given a state and a list of states, this function returns true if the given state is in the list of states.
  /// @param states - The list of states to compare with
  /// @param s - The state to compare with
  /// @return True if the given state is in the list of states, false otherwise
  template <typename Container>
  static bool containState(const Container & states, const state * s, bool considerInternalVariables)
  {
    return std::any_of(states.begin(), states.end(), [&s, &considerInternalVariables](const auto & state) {
      return state->isSame(s, considerInternalVariables);
    });
  }

  /// @brief Given a state and a list of states, this function returns the state that is most similar to the given state.
  /// @param current - The state to compare with
  /// @param states - The list of states to compare with
  /// @return The state that is most similar to the given state
  template <typename Container> static state * most_similar_state(const state * current, const Container & states)
  {
    bool considerInternalVariables = true;
    // Similarity is measured by the delta function in the range of [0, 1]
    auto most_similar = *std::min_element(
        states.begin(), states.end(), [&current, &considerInternalVariables](const auto & s1, const auto & s2) {
          return s1->delta(current, considerInternalVariables) < s2->delta(current, considerInternalVariables);
        });

    return most_similar;
  }

  template <typename Container> static state * most_different_state(const state * current, const Container & states)
  {
    bool considerInternalVariables = false;
    // Similarity is measured by the delta function in the range of [0, 1]
    auto most_different = *std::max_element(
        states.begin(), states.end(), [&current, &considerInternalVariables](const auto & s1, const auto & s2) {
          return s1->delta(current, considerInternalVariables) < s2->delta(current, considerInternalVariables);
        });

    return most_different;
  }

  /// @brief Given two lists of states, this function returns the states that are in the first list but not in the second list.
  /// @param states_original - The original list of states
  /// @param states_mutant - The list of states to compare with
  /// @return A list of states that are in the first list but not in the second list
  template <typename Container>
  static Container distinct_states(const Container & states_original, const Container & states_mutant)
  {
    Container distinct;
    bool considerInternalVariables = false;
    for (auto & original_state : states_original) {
      bool found = containState(states_mutant, original_state, considerInternalVariables);
      if (!found) {
        distinct.push_back(original_state);
      }
    }
    return distinct;
  }

  static successorTreeComparison compareKSuccessors(const successorTree & original_successors,
                                                    const successorTree & mutant_successors);
};