#ifndef EXPLORE_H
#define EXPLORE_H

#include <list>
#include <stack>

#include <daedalux/core/automata.hpp>
#include <daedalux/core/semantic/variable/state.hpp>
#include <daedalux/feature/tvl.hpp>
#include <daedalux/algorithm/elementStack.hpp>
#include <daedalux/algorithm/reachabilityRelation.hpp>
#include <daedalux/visualizer/stateToGraphViz.hpp>
#include <daedalux/visualizer/traceReport.hpp>

typedef char byte;
typedef unsigned char ubyte;

void printElementStack(stateToGraphViz * graphVis, const std::stack<std::shared_ptr<elementStack::element>> & outerStack,
                       const std::stack<std::shared_ptr<elementStack::element>> & innerStack =
                           std::stack<std::shared_ptr<elementStack::element>>(),
                       const state * loopBegin = nullptr);

void launchExecution(const fsm * automata, const TVL * tvl = nullptr);
int launchExecutionMarkovChain(const fsm * automata, const TVL * tvl = nullptr);
void createStateSpaceDFS(const fsm * automata, const TVL * tvl = nullptr);
void createStateSpaceBFS(const fsm * automata, const TVL * tvl = nullptr);
void createStateSpaceDFS_RR(const fsm * automata, const TVL * tvl = nullptr);

std::unique_ptr<trace> interactiveDebugging(const std::shared_ptr<fsm> automata, const size_t trace_length, const TVL * tvl);

#endif