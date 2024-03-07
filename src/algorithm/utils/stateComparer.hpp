#pragma once

#include "state.hpp"
#include <memory>
#include <vector>

class StateComparer {
public:
  static state * most_similar_state(const state * current, const std::list<state *> states);
  static std::list<state *> distinct_states(const std::list<state *> & states_original,
                                            const std::list<state *> & states_mutant);

  static std::map<unsigned int, std::vector<std::shared_ptr<state>>>
  compareKSuccessors(const std::map<unsigned int, std::vector<std::shared_ptr<state>>> & original_successors,
                     const std::map<unsigned int, std::vector<std::shared_ptr<state>>> & mutant_successors);
};