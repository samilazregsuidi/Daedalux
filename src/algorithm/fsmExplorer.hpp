#ifndef FSMEXPLORER_HPP
#define FSMEXPLORER_HPP

#include "../formulas/formula.hpp"
#include "fsm.hpp"
#include "initState.hpp"
#include "state.hpp"
#include "utils/successorTree.hpp"
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

class fsmExplorer {
public:
  static std::list<state *> avoidEpsilonSteps(state * start_state, unsigned int budget = 5);

  static std::shared_ptr<formula> discardMutant(std::shared_ptr<fsm> original, std::shared_ptr<fsm> mutant);

  static successorTree kSuccessors(state * start_state, unsigned int k);

  static void analyzeSuccessors(state * state_original, state * state_mutant, unsigned int k);
};

#endif // FSMEXPLORER_HPP