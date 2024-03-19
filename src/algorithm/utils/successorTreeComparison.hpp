#pragma once

#include "../../formulas/formulaUtility.hpp"
#include "state.hpp"
#include "successorTree.hpp"
#include <iostream>
#include <memory>
#include <vector>

// Enum to define whether or not the state is in the original or mutant tree
enum class TreeType { Original, Mutant };

class successorTreeComparison {
public:
  successorTreeComparison(const successorTree & original_only, const successorTree & mutant_only, const successorTree & common)
      : original_only(original_only), mutant_only(mutant_only), common(common)
  {
  }

  successorTree getOriginalOnly() const { return original_only; }
  successorTree getMutantOnly() const { return mutant_only; }
  successorTree getCommon() const { return common; }

  /// @brief A function to get the values of the original tree that are not in the mutant tree
  /// @return
  // std::map<std::string, std::vector<ValueType>> getOriginalOnlyValues() {
  //   auto variablesOriginal = formulaUtility::buildVariableValueMap(mapToValues(original_only));
  //   auto variablesMutant = formulaUtility::buildVariableValueMap(mapToValues(mutant_only));

  // }

  /// @brief Return the states that are only in the original tree
  /// @param None
  /// @return A vector of states that are only in the original tree
  std::vector<state *> getOriginalStates(void) const { return original_only.getStates(); }

  std::vector<state *> getMutantStates(void) const { return mutant_only.getStates(); }

  std::vector<state *> getCommonStates(void) const { return common.getStates(); }

  /// @brief A function to get the states where the predicate is true
  /// @param type defines the tree to check - original or mutant
  /// @param pred a predicate function to check
  /// @return a vector of states where the predicate is true
  std::vector<state *> statesWhere(TreeType type, Predicate pred) const
  {
    std::vector<state *> result;
    auto states = type == TreeType::Original ? getOriginalStates() : getMutantStates();
    std::copy_if(states.begin(), states.end(), std::back_inserter(result), pred);
    return result;
  }

  /// @brief Check if the predicate is invariant for the given tree
  /// @param type defines the tree to check - original or mutant
  /// @param pred a predicate function to check
  /// @return true if the predicate is invariant, false otherwise
  bool isInvariant(TreeType type, Predicate pred) const
  {
    auto states = type == TreeType::Original ? getOriginalStates() : getMutantStates();
    auto result = std::all_of(states.begin(), states.end(), pred);
    return result;
  }

  /// @brief Check if the two trees are equal
  /// @param
  /// @return Return true if the trees are equal, false otherwise
  bool areEqual(void) const { return original_only.empty() && mutant_only.empty(); }

  bool areDifferent(void) const { return !areEqual(); }

private:
  successorTree original_only;
  successorTree mutant_only;
  successorTree common;
};
