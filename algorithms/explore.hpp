#ifndef EXPLORE_H
#define EXPLORE_H

#include <stack>
#include <list>

#include "automata.hpp"
#include "state.hpp"
#include "tvl.hpp"

#include "elementStack.hpp"
#include "reachabilityRelation.hpp"

typedef char byte;
typedef unsigned char ubyte;

#include "stateVisitor.hpp"

class ltlModelChecker {
public:
    void startNestedDFS(const fsm* automata, const TVL* tvl);
    byte outerDFS(elementStack& stackOuter);
    byte innerDFS(elementStack& stackInner, const elementStack& stackOuter);

public:
    reachabilityRelation R;
};

void printElementStack(const std::stack<elementStack::element*>& outerStack, const std::stack<elementStack::element*>& innerStack = std::stack<elementStack::element*>(), const state* loopBegin = nullptr);

void launchExecution(const fsm* automata, const TVL* tvl = nullptr);
void createStateSpaceDFS(const fsm* automata, const TVL* tvl = nullptr);
void createStateSpaceBFS(const fsm* automata, const TVL* tvl = nullptr);
void createStateSpaceDFS_RR(const fsm* automata, const TVL* tvl = nullptr);

#endif