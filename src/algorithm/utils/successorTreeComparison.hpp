#pragma once

#include "state.hpp"
#include <iostream>
#include <memory>
#include <vector>

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

  bool areEqual(void) { return original_only.empty() && mutant_only.empty(); }

  bool areDifferent(void) { return !original_only.empty() || !mutant_only.empty(); }

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
