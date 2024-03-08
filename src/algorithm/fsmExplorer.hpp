#ifndef FSMEXPLORER_HPP
#define FSMEXPLORER_HPP

#include "../formulas/formula.hpp"
#include "fsm.hpp"
#include <map>
#include <memory>
#include <state.hpp>
#include <string>
#include <vector>

class fsmExplorer {
public:
  static std::shared_ptr<formula> discardMutant(std::shared_ptr<fsm> original, std::shared_ptr<fsm> mutant);

  static std::map<unsigned int, std::vector<state *>> kSuccessors(state * start_state, unsigned int k);
};

#endif // FSMEXPLORER_HPP
