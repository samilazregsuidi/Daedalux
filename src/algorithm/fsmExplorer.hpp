#ifndef FSMEXPLORER_HPP
#define FSMEXPLORER_HPP

#include "../formulas/formula.hpp"
#include "fsm.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <state.hpp>

class fsmExplorer {
public:
  static std::shared_ptr<formula> discardMutant(std::shared_ptr<fsm> original, std::shared_ptr<fsm> mutant);

  static std::map<unsigned int, std::vector<std::shared_ptr<state>>> kSuccessors(std::shared_ptr<state> start_state,
                                                                                 unsigned int k);
};

#endif // FSMEXPLORER_HPP
