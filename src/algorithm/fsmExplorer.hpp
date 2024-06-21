#ifndef FSMEXPLORER_HPP
#define FSMEXPLORER_HPP

#include "../formulas/formula.hpp"
#include "../formulas/formulaCreator.hpp"
#include "fsm.hpp"
#include "initState.hpp"
#include "state.hpp"
#include "utils/stateComparer.hpp"
#include "utils/successorTree.hpp"
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

class fsmExplorer {
public:
  static std::list<state *> avoidEpsilonSteps(const state * start_state, unsigned int budget = 5);

  static std::shared_ptr<formula> discardMutant(std::shared_ptr<fsm> original, std::shared_ptr<fsm> mutant);

  static successorTree kSuccessors(state * start_state, unsigned int k);

  static void analyzeSuccessors(state * state_original, state * state_mutant, unsigned int k);

  static bool checkFormula(std::shared_ptr<formula> f, const std::string & original_file, const std::string & mutant_file);

  // Method to find the unique successor
  template <typename Container>
  static std::shared_ptr<formula> findUniqueSuccessorFormula(Container & unique_successors, Container & successors_current,
                                                             const Container & successors_other, state * current_state,
                                                             unsigned int budget, bool unwantedTransition = false)
  {
    // Check if the current state is different from its successor states
    bool stateDifferent = StateComparer::containState(unique_successors, current_state, false);
    if (stateDifferent) {
      // Find a successor state that is not the same as the current state
      auto differentSuccessors = avoidEpsilonSteps(current_state, budget);
      // Assuming distinct_states is a static method of StateComparer
      auto unique_successors = StateComparer::distinct_states(differentSuccessors, successors_current);
    }
    // Handle the case where no unique successor is found, perhaps by returning a null pointer or an optional
    std::vector<std::shared_ptr<state>> successors_current_vec;
    std::vector<std::shared_ptr<state>> successors_other_vec;
    for (auto s : successors_current) {
      successors_current_vec.push_back(std::shared_ptr<state>(s));
    }
    for (auto s : successors_other) {
      successors_other_vec.push_back(std::shared_ptr<state>(s));
    }

    auto shared_current_state = std::shared_ptr<state>(current_state);
    // Assuming createTransitionFormula returns a shared_ptr to a Formula object
    auto distinguishing_formula =
        formulaCreator::createTransitionFormula(shared_current_state, successors_current_vec, successors_other_vec, unwantedTransition);

    return distinguishing_formula;
  }
};

#endif // FSMEXPLORER_HPP