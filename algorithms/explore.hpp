#ifndef EXPLORE_H
#define EXPLORE_H

#include <stack>
#include <list>

#include "automata.hpp"
#include "state.hpp"
#include "tvl.hpp"

typedef char byte;
typedef unsigned char ubyte;

#include "stateVisitor.hpp"

struct element {
	element(void)
        : s(nullptr)
        , init(false)
    {}
    
    element(state* s, unsigned int depth = 0)
		: s(s)
        , init(false)
        , depth(depth)
	{}

    ~element() {
        if(s)
            delete s;
        for(auto p : Post)
            delete p;
    }

	state* s;
	std::list<state*> Post;
    bool init;
    unsigned int depth;
};

enum DFS {
    OUTER,
    INNER
};

void printElementStack(const std::stack<element*>& outerStack, const std::stack<element*>& innerStack = std::stack<element*>(), const state* loopBegin = nullptr);

struct htState {
    
    htState(unsigned long hash)
        : hash(hash)
        , outerFeatures(TVL::getMgr()->addOne())
        , innerFeatures(TVL::getMgr()->addOne())
        , foundIn(OUTER)
    {
    }
    
    htState(unsigned long hash, const ADD& outerFeatures, const ADD& innerFeatures) 
        : hash(hash)
        , outerFeatures(outerFeatures)
        , innerFeatures(innerFeatures)
        , foundIn(OUTER)
    {
    }

    ~htState() {
        for(auto subS : subStates) {
            delete subS;
        }
    }

    htState* getSubHtState(unsigned long hash) {
        for(auto htS : subStates)  {
            if(htS->hash == hash)
                return htS;
        }
        return nullptr;
    }

    unsigned long hash;
    ADD outerFeatures;
    ADD innerFeatures;
    DFS foundIn;

    std::list<htState*> subStates;
};

class reachabilityRelation : public stateVisitor {
public:

    reachabilityRelation(DFS dfs, std::map<unsigned long, htState*>& map);

    ~reachabilityRelation() override;

    byte updateReachability(state* s);

    void visit(state* s) override;
	void visit(process* s) override;
	void visit(progState* s) override;
	void visit(compState* s) override;
    void visit(never* s) override;
	void visit(featStateDecorator* s) override;

public:
    DFS dfs;
    std::map<unsigned long, htState*>& map; 
    htState* current;
    byte res;
};

class stateToHTState : public stateVisitor {
public:
    stateToHTState(state* s);
    void visit(state* s) override;
	void visit(process* s) override;
	void visit(progState* s) override;
	void visit(compState* s) override;
    void visit(never* s) override;
	void visit(featStateDecorator* s) override;
    operator htState*(void) const;
public:
    htState* res;
};

void launchExecution(const fsm* automata, const TVL* tvl = nullptr);
void createStateSpaceDFS(const fsm* automata, const TVL* tvl = nullptr);
void createStateSpaceBFS(const fsm* automata, const TVL* tvl = nullptr);

void startNestedDFS(const fsm* automata, const TVL* tvl);
byte outerDFS(std::stack<element*>& stackOuter);
byte innerDFS(std::stack<element*>& stackInner, const std::stack<element*>& stackOuter, std::map<unsigned long, htState*>& map);

#endif