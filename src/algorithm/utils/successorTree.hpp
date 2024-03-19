#pragma once

#include "../../formulas/formulaCreator.hpp"
#include "../../formulas/formulaUtility.hpp"
#include "state.hpp"
#include <functional> // For std::function
#include <iostream>
#include <memory>
#include <vector>

class successorTree {
public:
  explicit successorTree(const std::map<unsigned int, std::vector<state *>> & states) : states(states) {}

  unsigned int numStates() const { return getStates().size(); }

  unsigned int length() const { return states.size(); }

  bool empty() const { return states.empty(); }

  std::map<unsigned int, std::vector<state *>> getStateTree() const { return states; }

  /// @brief Get the initial state of the tree
  /// @return The initial state of the tree
  const state * getInitialState() { return states[0][0]; }

  /// @brief Unpack the map of states into a vector of states
  /// @return a vector of all the states in the tree
  std::vector<state *> getStates() const
  {
    std::vector<state *> result;
    for (auto st : states) {
      std::copy(st.second.begin(), st.second.end(), std::back_inserter(result));
    }
    return result;
  }

  /// @brief Generate a formula based on the states in the tree
  /// @return a formula based on the states in the tree
  std::shared_ptr<formula> generateFormula()
  {
    auto allStates = getStates();
    auto state_ptrs = std::vector<std::shared_ptr<state>>(allStates.begin(), allStates.end());
    if (state_ptrs.empty()) {
      return std::make_shared<BooleanConstant>(false);
    }
    auto form = formulaCreator::groupStatesByFormula(state_ptrs);
    return form;
  }

  /// @brief A function to get the states where the predicate is true
  /// @param type defines the tree to check - original or mutant
  /// @param pred a predicate function to check
  /// @return a vector of states where the predicate is true
  std::vector<state *> statesWhere(Predicate pred) const
  {
    std::vector<state *> result;
    auto allStates = getStates();
    std::copy_if(allStates.begin(), allStates.end(), std::back_inserter(result), pred);
    return result;
  }

  /// @brief Check if any of the states in the tree satisfy the predicate
  /// @param pred a predicate function over a state to check
  /// @return true if any of the states satisfy the predicate, false otherwise
  bool any(Predicate pred)
  {
    auto allStates = getStates();
    return std::any_of(allStates.begin(), allStates.end(), pred);
  }

  /// @brief Check if the predicate is invariant for the given tree
  /// @param pred a predicate function to check
  /// @return true if the predicate is invariant, false otherwise
  bool isInvariant(Predicate pred)
  {
    auto allStates = getStates();
    auto result = statesWhere(pred);
    return result.size() == allStates.size();
  }

private:
  std::map<unsigned int, std::vector<state *>> states;
};
