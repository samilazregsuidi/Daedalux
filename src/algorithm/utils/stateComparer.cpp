#include "stateComparer.hpp"

/// @brief Given a state and a list of states, this function returns true if the given state is in the list of states.
/// @param states - The list of states to compare with
/// @param s - The state to compare with
/// @return True if the given state is in the list of states, false otherwise
bool StateComparer::containState(const std::vector<std::shared_ptr<state>> & states, const state * s)
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

std::vector<std::shared_ptr<state>> StateComparer::findDistinctStates(std::list<state *> list1, std::list<state *> list2)
{
  auto states_only_list1 = distinct_states(list1, list2);
  auto list1_only_states_vector = std::vector<std::shared_ptr<state>>();
  for (auto & s : states_only_list1) {
    list1_only_states_vector.push_back(std::shared_ptr<state>(s));
  }
  return list1_only_states_vector;
}

/// @brief Given two maps of states, this function returns the states that are in the first map but not in the second map.
/// @param original_successors - The original map of states
/// @param mutant_successors - The map of states to compare with
/// @return An object telling which states are only in the original map, only in the mutant map, and common to both maps
successorTreeComparison
StateComparer::compareKSuccessors(const std::map<unsigned int, std::vector<std::shared_ptr<state>>> & original_successors,
                                  const std::map<unsigned int, std::vector<std::shared_ptr<state>>> & mutant_successors)
{
  // Ensure that the two maps have the same keys
  if (original_successors.size() != mutant_successors.size()) {
    throw std::runtime_error("The two maps to compare do not have the same number of keys");
  }

  std::map<unsigned int, std::vector<std::shared_ptr<state>>> original_only;
  std::map<unsigned int, std::vector<std::shared_ptr<state>>> mutant_only;
  std::map<unsigned int, std::vector<std::shared_ptr<state>>> common;
  for (auto & original : original_successors) {
    auto key = original.first;
    auto original_states = original.second;
    auto original_states_list = std::list<state *>();
    for (auto & s : original_states) {
      original_states_list.push_back(s.get());
    }
    auto mutant_states = mutant_successors.at(key);
    auto mutant_states_list = std::list<state *>();
    for (auto & s : mutant_states) {
      mutant_states_list.push_back(s.get());
    }
    auto original_only_states = findDistinctStates(original_states_list, mutant_states_list);
    auto mutant_only_states = findDistinctStates(mutant_states_list, original_states_list);
    auto common_states = std::vector<std::shared_ptr<state>>();
    for (auto & s : original_states) {
      if (StateComparer::containState(mutant_states, s.get())) {
        common_states.push_back(s);
      }
    }
    original_only[key] = original_only_states;
    mutant_only[key] = mutant_only_states;
    common[key] = common_states;
  }
  auto result = successorTreeComparison{original_only, mutant_only, common};
  return result;
}
