#ifndef EXPLORE_H
#define EXPLORE_H

#include <list>
#include <stack>

#include "automata.hpp"
#include "state.hpp"
#include "tvl.hpp"

#include "elementStack.hpp"
#include "reachabilityRelation.hpp"
#include "traceReport.hpp"

typedef char byte;
typedef unsigned char ubyte;

#include "stateVisitor.hpp"

class ltlModelChecker {
public:
  void startNestedDFS(const fsm * automata, const TVL * tvl);
  byte outerDFS(elementStack & stackOuter);
  byte innerDFS(elementStack & stackInner, const elementStack & stackOuter);

  reachabilityRelation R;
};

void printElementStack(const std::stack<std::shared_ptr<elementStack::element>> & outerStack,
                       const std::stack<std::shared_ptr<elementStack::element>> & innerStack =
                           std::stack<std::shared_ptr<elementStack::element>>(),
                       const state * loopBegin = nullptr);

void launchExecution(const fsm * automata, const TVL * tvl = nullptr);
int launchExecutionMarkovChain(const fsm * automata, const TVL * tvl = nullptr);
void createStateSpaceDFS(const fsm * automata, const TVL * tvl = nullptr);
void createStateSpaceBFS(const fsm * automata, const TVL * tvl = nullptr);
void createStateSpaceDFS_RR(const fsm * automata, const TVL * tvl = nullptr);
std::unique_ptr<trace> generateNegativeTraces(const std::shared_ptr<fsm> original, const std::shared_ptr<fsm> mutant,
                                              const size_t k = 200, const TVL * tvl = nullptr);
std::unique_ptr<traceReport> generateTraces(const std::shared_ptr<fsm> original, const std::shared_ptr<fsm> mutant,
                                            const size_t no_traces = 20, const size_t len_traces = 200,
                                            const TVL * tvl = nullptr);

transition * most_similar_transition(const std::list<transition *> transitions, const transition * current);
state * most_similar_state(const state * current, const std::list<state *> states);

std::list<state *> distinct_states(const std::list<state *> & states_original, const std::list<state *> & states_mutant);

#endif