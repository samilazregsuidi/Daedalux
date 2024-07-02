#ifndef EXPLORE_H
#define EXPLORE_H

#include <list>
#include <stack>

#include "automata.hpp"
#include "state.hpp"
#include "tvl.hpp"

#include "elementStack.hpp"
#include "reachabilityRelation.hpp"
#include "stateToGraphViz.hpp"
#include "traceReport.hpp"

typedef char byte;
typedef unsigned char ubyte;

#include "stateVisitor.hpp"

void printElementStack(stateToGraphViz * graphVis, const std::stack<std::shared_ptr<elementStack::element>> & outerStack,
                       const std::stack<std::shared_ptr<elementStack::element>> & innerStack =
                           std::stack<std::shared_ptr<elementStack::element>>(),
                       const state * loopBegin = nullptr);

void launchExecution(const fsm * automata, const TVL * tvl = nullptr);
int launchExecutionMarkovChain(const fsm * automata, const TVL * tvl = nullptr);
void createStateSpaceDFS(const fsm * automata, const TVL * tvl = nullptr);
void createStateSpaceBFS(const fsm * automata, const TVL * tvl = nullptr);
void createStateSpaceDFS_RR(const fsm * automata, const TVL * tvl = nullptr);

std::unique_ptr<trace> interactiveDebugging(const std::shared_ptr<fsm>& automata, const size_t trace_length, const TVL * tvl);

#endif