#include "stateComparer.hpp"


/// @brief Given two vector of states, this function returns true if the two vectors contain the same states. The order of the states does not matter.
/// @param s1 - The first vector of states
/// @param s2 - The second vector of states
/// @return True if the two vectors contain the same states, false otherwise
bool StateComparer::sameStates(const std::vector<std::shared_ptr<state>> & states1, const std::vector<std::shared_ptr<state>> & states2)
{
  if (states1.size() != states2.size()) {
    return false;
  }
  for (size_t i = 0; i < states1.size(); i++) {
    auto state1 = states1[i];
    bool isFound = false;
    for (size_t j = 0; j < states2.size(); j++) {
      auto state2 = states2[j];
      if (state1->isSame(state2.get())) {
        isFound = true;
        break;
      }
    }
    if (!isFound) {
      return false;
    }
  }
  return true;
}

/// @brief Given a state and a list of states, this function returns true if the given state is in the list of states.
/// @param states - The list of states to compare with
/// @param s - The state to compare with
/// @return True if the given state is in the list of states, false otherwise
bool StateComparer::containState(const std::vector<state *> & states, const state * s)
{
  for (auto & state : states) {
    if (state->isSame(s)) {
      return true;
    }
  }
  return false;
}

/// @brief Given a state and a list of states, this function returns the state that is most similar to the given state.
/// @param current - The state to compare with
/// @param states - The list of states to compare with
/// @return The state that is most similar to the given state
state * StateComparer::most_similar_state(const state * current, const std::list<state *> states)
{
  state * most_similar = nullptr;
  // Similarity is measured by the delta function in the range of [0, 1]
  double max_similarity = 1;
  for (auto s : states) {
    double delta = s->delta(current);
    if (delta < max_similarity) {
      max_similarity = delta;
      most_similar = s;
    }
  }
  return most_similar;
}

/// @brief Given two lists of states, this function returns the states that are in the first list but not in the second list.
/// @param states_original - The original list of states
/// @param states_mutant - The list of states to compare with
/// @return A list of states that are in the first list but not in the second list
std::list<state *> StateComparer::distinct_states(const std::list<state *> & states_original,
                                                  const std::list<state *> & states_mutant)
{
  std::list<state *> distinct;
  for (auto & original_state : states_original) {
    bool found = false;
    for (auto & mutant_state : states_mutant) {
      if (original_state->isSame(mutant_state)) {
        found = true;
        break;
      }
    }
    if (!found) {
      distinct.push_back(original_state);
    }
  }
  return distinct;
}

std::vector<state *> StateComparer::findDistinctStates(std::list<state *> list1, std::list<state *> list2)
{
  auto states_only_list1 = distinct_states(list1, list2);
  std::vector<state *> list1_only_states_vector(states_only_list1.begin(), states_only_list1.end());
  return list1_only_states_vector;
}

/// @brief Given two maps of states, this function returns the states that are in the first map but not in the second map.
/// @param original_successors - The original map of states
/// @param mutant_successors - The map of states to compare with
/// @return An object telling which states are only in the original map, only in the mutant map, and common to both maps
successorTreeComparison
StateComparer::compareKSuccessors(const std::map<unsigned int, std::vector<state *>> & original_successors,
                                  const std::map<unsigned int, std::vector<state *>> & mutant_successors)
{
  // Verify each key in original_successors exists in mutant_successors
  for (const auto & entry : original_successors) {
    if (mutant_successors.find(entry.first) == mutant_successors.end()) {
      throw std::runtime_error("Key mismatch between maps");
    }
  }
  std::map<unsigned int, std::vector<state *>> original_only;
  std::map<unsigned int, std::vector<state *>> mutant_only;
  std::map<unsigned int, std::vector<state *>> common;
  for (auto & original : original_successors) {
    auto key = original.first;
    const auto original_states = original.second;
    const auto mutant_states = mutant_successors.at(key);

    std::list<state *> original_states_list(original_states.begin(), original_states.end());
    std::list<state *> mutant_states_list(mutant_states.begin(), mutant_states.end());

    auto original_only_states = findDistinctStates(original_states_list, mutant_states_list);
    auto mutant_only_states = findDistinctStates(mutant_states_list, original_states_list);
    auto common_states = std::vector<state *>();
    for (auto & s : original_states) {
      if (StateComparer::containState(mutant_states, s)) {
        common_states.push_back(s);
      }
    }
    original_only[key] = std::move(original_only_states);
    mutant_only[key] = std::move(mutant_only_states);
    common[key] = std::move(common_states);
  }
  auto result = successorTreeComparison{original_only, mutant_only, common};
  return result;
}
