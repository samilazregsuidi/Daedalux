#ifndef BisimulationChecker_H
#define BisimulationChecker_H

#include <memory>
#include <daedalux/core/automata/fsm.hpp>
#include <daedalux/feature/tvl.hpp>

class BisimulationChecker {

public:
  static bool isBisimilar(const std::shared_ptr<fsm> fsm1, const std::shared_ptr<fsm> fsm2, const TVL * tvl = nullptr);

  static void getDistinctStates(const std::shared_ptr<fsm> fsm1, const std::shared_ptr<fsm> fsm2);
};

#endif // BisimulationChecker_H