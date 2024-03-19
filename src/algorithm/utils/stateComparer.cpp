#include "stateComparer.hpp"

/// @brief Given two vector of states, this function returns true if the two vectors contain the same states. The order of the
/// states does not matter.
/// @param s1 - The first vector of states
/// @param s2 - The second vector of states
/// @return True if the two vectors contain the same states, false otherwise
bool StateComparer::sameStates(const std::vector<std::shared_ptr<state>> & states1,
                               const std::vector<std::shared_ptr<state>> & states2, bool considerInternalVariables)
{
  auto result1 = std::all_of(states1.begin(), states1.end(), [&states2, &considerInternalVariables](const auto & state1) {
    return containState(states2, state1.get(), considerInternalVariables);
  });
  auto result2 = std::all_of(states2.begin(), states2.end(), [&states1, &considerInternalVariables](const auto & state2) {
    return containState(states1, state2.get(), considerInternalVariables);
  });
  return result1 && result2;
}

/// @brief Given a list of formulas and a formula, this function returns true if the list of formulas contains the given
/// formula.
/// @param formulas - The list of formulas to compare with
/// @param formula - The formula to compare with
/// @return True if the list of formulas contains the given formula, false otherwise
bool StateComparer::containsFormula(const std::vector<std::shared_ptr<formula>> & formulas,
                                    const std::shared_ptr<formula> & formula)
{
  return std::any_of(formulas.begin(), formulas.end(), [&formula](const auto & f) { return f->isEquivalent(*formula); });
}

std::vector<std::shared_ptr<formula>> StateComparer::removeDuplicates(const std::vector<std::shared_ptr<formula>> & formulas)
{
  std::vector<std::shared_ptr<formula>> unique_formulas;
  for (auto & formula : formulas) {
    if (!containsFormula(unique_formulas, formula)) {
      unique_formulas.push_back(formula);
    }
  }
  return unique_formulas;
}

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
    for (auto & s : original_states) {
      if (containState(mutant_states, s, considerInternalVariables)) {
        common_states.push_back(s);
      }
    }
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
