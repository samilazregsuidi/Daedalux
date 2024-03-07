#pragma once

#include <list>
#include <stack>

#include "automata.hpp"
#include "state.hpp"
#include "tvl.hpp"

#include "elementStack.hpp"
#include "reachabilityRelation.hpp"
#include <stateToGraphViz.hpp>

typedef char byte;
typedef unsigned char ubyte;

class ltlModelChecker {
public:
  bool check(const fsm * automata, const TVL * tvl, bool generateIntermediaryFiles = false);
  byte outerDFS(elementStack & stackOuter, bool generateIntermediaryFiles = false);
  byte innerDFS(elementStack & stackInner, const elementStack & stackOuter, bool generateIntermediaryFiles = false);

  reachabilityRelation getReachabilityRelation() const { return reachableStates; }

private:
  reachabilityRelation reachableStates;
  stateToGraphViz * graphVis;
  unsigned long nbStatesExplored = 1; // Total of distinct states (without features) explored
  unsigned long nbStatesReExplored =
      0; // Total of states that had to be re-explored because new products were found to be able to reach them
  unsigned long nbStatesStops = 0; // Total of states where exploration backtracked because they were already visited before
  unsigned long nbStatesExploredInner = 0;   // As before, but for inner search.
  unsigned long nbStatesReExploredInner = 0; // As before, but for inner search.
  unsigned long depth = 0;                   // Current exploration depth (inner and outer)

  void checkForDeadlock(std::shared_ptr<state> state, const elementStack & stack, bool printStack);
  void checkNeverClaim(std::shared_ptr<state> init);
  void resetCounters();
  void emptyStack(elementStack & stack);
};