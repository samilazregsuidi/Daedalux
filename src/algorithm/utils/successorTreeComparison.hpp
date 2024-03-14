#pragma once

#include "state.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <functional> // For std::function

// Define the type for the predicate function
using Predicate = std::function<bool(state *)>;

// Enum to define whether or not the state is in the original or mutant tree
enum class TreeType { Original, Mutant };

class successorTreeComparison {
public:
  successorTreeComparison(std::map<unsigned int, std::vector<state *>> original_only,
                          std::map<unsigned int, std::vector<state *>> mutant_only,
                          std::map<unsigned int, std::vector<state *>> common)
      : original_only(original_only), mutant_only(mutant_only), common(common)
  {
  }

  std::map<unsigned int, std::vector<state *>> getOriginalOnly() { return original_only; }
  std::map<unsigned int, std::vector<state *>> getMutantOnly() { return mutant_only; }
  std::map<unsigned int, std::vector<state *>> getCommon() { return common; }

  std::vector<state *> getOriginalStates(void) { return mapToValues(original_only); }

  std::vector<state *> getMutantStates(void) { return mapToValues(mutant_only); }

  std::vector<state *> getCommonStates(void) { return mapToValues(common); }

  /// @brief A function to get the states where the predicate is true
  /// @param type defines the tree to check - original or mutant
  /// @param pred a predicate function to check
  /// @return a vector of states where the predicate is true
  std::vector<state *> statesWhere(TreeType type, Predicate pred){
    std::vector<state *> result;
    auto states = type == TreeType::Original ? getOriginalStates() : getMutantStates();
    for(auto st : states){
      if(pred(st)){
        result.push_back(st);
      }
    }
    return result;
  }

  /// @brief Check if the predicate is invariant for the given tree
  /// @param type defines the tree to check - original or mutant
  /// @param pred a predicate function to check
  /// @return true if the predicate is invariant, false otherwise
  bool isInvariant(TreeType type, Predicate pred){
    auto states = type == TreeType::Original ? getOriginalStates() : getMutantStates();
    auto result = statesWhere(type, pred);
    return result.size() == states.size();
  }

  /// @brief Check if the two trees are equal
  /// @param  
  /// @return Return true if the trees are equal, false otherwise
  bool areEqual(void) { return original_only.empty() && mutant_only.empty(); }

  bool areDifferent(void) { return !areEqual(); }

  void print(void)
  {
    auto min_depth = std::min({original_only.rbegin()->first, mutant_only.rbegin()->first, common.rbegin()->first});
    for (unsigned int i = 0; i <= min_depth; i++) {
      auto original = original_only.find(i);
      auto mutant = mutant_only.find(i);
      auto common_states = common.find(i);

      std::cout << "Depth " << i << ": ";
      std::cout << "Original only: ";
      for (auto state : original_only[i]) {
        state->print();
      }
      std::cout << "Mutant only: ";
      for (auto state : mutant_only[i]) {
        state->print();
      }
      std::cout << "Common: ";
      for (auto state : common[i]) {
        state->print();
      }
      std::cout << std::endl;
    }
  }

private:
  std::map<unsigned int, std::vector<state *>> original_only;
  std::map<unsigned int, std::vector<state *>> mutant_only;
  std::map<unsigned int, std::vector<state *>> common;

  std::vector<state *> mapToValues(std::map<unsigned int, std::vector<state *>> & map)
  {
    std::vector<state *> values;
    for (auto it = map.begin(); it != map.end(); it++) {
      for (auto state : it->second) {
        values.push_back(state);
      }
    }
    return values;
  };
};
