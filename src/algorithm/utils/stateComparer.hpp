#pragma once

#include "state.hpp"
#include <memory>
#include <vector>

class successorTreeComparison {
public:
  successorTreeComparison(std::map<unsigned int, std::vector<std::shared_ptr<state>>> original_only,
                          std::map<unsigned int, std::vector<std::shared_ptr<state>>> mutant_only,
                          std::map<unsigned int, std::vector<std::shared_ptr<state>>> common)
      : original_only(original_only), mutant_only(mutant_only), common(common)
  {
  }

  std::map<unsigned int, std::vector<std::shared_ptr<state>>> getOriginalOnly() { return original_only; }
  std::map<unsigned int, std::vector<std::shared_ptr<state>>> getMutantOnly() { return mutant_only; }
  std::map<unsigned int, std::vector<std::shared_ptr<state>>> getCommon() { return common; }

  std::vector<std::shared_ptr<state>> getOriginalStates(void) { return mapToValues(original_only); }

  std::vector<std::shared_ptr<state>> getMutantStates(void) { return mapToValues(mutant_only); }

  std::vector<std::shared_ptr<state>> getCommonStates(void) { return mapToValues(common); }

private:
  std::map<unsigned int, std::vector<std::shared_ptr<state>>> original_only;
  std::map<unsigned int, std::vector<std::shared_ptr<state>>> mutant_only;
  std::map<unsigned int, std::vector<std::shared_ptr<state>>> common;

  std::vector<std::shared_ptr<state>> mapToValues(std::map<unsigned int, std::vector<std::shared_ptr<state>>> & map)
  {
    std::vector<std::shared_ptr<state>> values;
    for (auto it = map.begin(); it != map.end(); it++) {
      for (auto state : it->second) {
        values.push_back(state);
      }
    }
    return values;
  };
};

class StateComparer {
public:
  static bool containState(const std::vector<std::shared_ptr<state>> & states, const state * s);

  static state * most_similar_state(const state * current, const std::list<state *> states);
  static std::list<state *> distinct_states(const std::list<state *> & states_original,
                                            const std::list<state *> & states_mutant);

  static std::vector<std::shared_ptr<state>> findDistinctStates(std::list<state *> list1, std::list<state *> list2);

  static successorTreeComparison
  compareKSuccessors(const std::map<unsigned int, std::vector<std::shared_ptr<state>>> & original_successors,
                     const std::map<unsigned int, std::vector<std::shared_ptr<state>>> & mutant_successors);
};