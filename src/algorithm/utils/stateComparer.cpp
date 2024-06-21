#include "stateComparer.hpp"

/// @brief Given two maps of states, this function returns the states that are in the first map but not in the second map.
/// @param original_successors - The original map of states
/// @param mutant_successors - The map of states to compare with
/// @return An object telling which states are only in the original map, only in the mutant map, and common to both maps
successorTreeComparison StateComparer::compareKSuccessors(const successorTree & original_tree,
                                                          const successorTree & mutant_tree)
{
  std::map<unsigned int, std::vector<state *>> original_only;
  std::map<unsigned int, std::vector<state *>> mutant_only;
  std::map<unsigned int, std::vector<state *>> common;
  auto original_successors = original_tree.getStateTree();
  auto mutant_successors = mutant_tree.getStateTree();
  auto shortest = std::min(original_successors.size(), mutant_successors.size());
  auto longest = std::max(original_successors.size(), mutant_successors.size());
  // We don't consider internal variables when comparing states
  bool considerInternalVariables = false;
  for (unsigned int i = 0; i < shortest; i++) {
    auto original_states = original_successors.at(i);
    auto mutant_states = mutant_successors.at(i);

    auto original_only_states = distinct_states(original_states, mutant_states);
    auto mutant_only_states = distinct_states(mutant_states, original_states);

    auto common_states = std::vector<state *>();
    std::copy_if(original_states.begin(), original_states.end(), std::back_inserter(common_states),
                 [&mutant_states, &considerInternalVariables](const auto & s) {
                   return containState(mutant_states, s, considerInternalVariables);
                 });
    original_only[i] = std::move(original_only_states);
    mutant_only[i] = std::move(mutant_only_states);
    common[i] = std::move(common_states);
  }
  for (unsigned int i = shortest; i < longest; i++) {
    if (original_successors.find(i) != original_successors.end()) {
      original_only[i] = original_successors.at(i);
    }
    if (mutant_successors.find(i) != mutant_successors.end()) {
      mutant_only[i] = mutant_successors.at(i);
    }
  }
  auto commonTree = successorTree(common);
  auto original_only_tree = successorTree(original_only);
  auto mutant_only_tree = successorTree(mutant_only);
  auto result = successorTreeComparison(original_only_tree, mutant_only_tree, commonTree);
  return result;
}
