#pragma once

#include "../../formulas/formula.hpp"
#include "state.hpp"
#include "successorTreeComparison.hpp"
#include <memory>
#include <vector>

/// @brief This class is responsible for comparing states and trees of states using a bunch of static methods
class StateComparer {
public:
  static bool sameStates(const std::vector<std::shared_ptr<state>> & states1,
                         const std::vector<std::shared_ptr<state>> & states2, bool considerInternalVariables);

  static bool containsFormula(const std::vector<std::shared_ptr<formula>> & formulas, const std::shared_ptr<formula> & formula);

  static std::vector<std::shared_ptr<formula>> removeDuplicates(const std::vector<std::shared_ptr<formula>> & formulas);

  static bool containState(const std::vector<state *> & states, const state * s, bool considerInternalVariables);

  static state * most_similar_state(const state * current, const std::list<state *> states);
  static std::list<state *> distinct_states(const std::list<state *> & states_original,
                                            const std::list<state *> & states_mutant);

  static std::vector<state *> findDistinctStates(std::list<state *> list1, std::list<state *> list2);

  static successorTreeComparison compareKSuccessors(const std::map<unsigned int, std::vector<state *>> & original_successors,
                                                    const std::map<unsigned int, std::vector<state *>> & mutant_successors);
};