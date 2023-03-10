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
    
    element(state* s)
		: s(s)
        , init(false)
	{}

    ~element() {
        if(s) 
            delete s;
        for(auto p : Post)
            delete p;
        /*for(auto p : Post_save)
            delete p;*/
    }

	state* s;
	std::list<state*> Post;
	//std::list<state*> Post_save;
    bool init;
};

struct htState {
    
    htState(state* s = nullptr)
        : s(s)
        , outerFeatures(TVL::getMgr()->addOne())
        , innerFeatures(TVL::getMgr()->addOne())
    {}
    
    htState(state* s, const ADD& outerFeatures, const ADD& innerFeatures) 
        : s(s)
        , outerFeatures(outerFeatures)
        , innerFeatures(innerFeatures)
    {}

    htState* getSubHtState(unsigned long hash) {
        for(auto htS : subStates)  {
            if(s->hash() == hash)
                return htS;
        }
        return nullptr;
    }

    state* s;
    ADD outerFeatures;
    ADD innerFeatures;

    std::list<htState*> subStates;
};

class reachabilityRelation : public stateVisitor {
public:
    enum DFS {
        OUTER,
        INNER
    };
    reachabilityRelation(DFS dfs);

    ~reachabilityRelation();

    byte updateReachability(state* s);

    void visit(state* s) override;
	void visit(process* s) override;
	void visit(progState* s) override;
	void visit(compState* s) override;
    void visit(never* s) override;
	void visit(featStateDecorator* s) override;

public:
    DFS dfs;
    std::map<unsigned long, htState*> outerHT; 
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
void createStateSpace(const fsm* automata, const TVL* tvl = nullptr);
void countStates(const fsm* automata, const TVL* tvl = nullptr);

void startNestedDFS(const fsm* automata, const TVL* tvl);
byte outerDFS(std::stack<element*>& stackOuter);
byte innerDFS(std::stack<element*>& stackInner, const std::map<unsigned long, htState*>& outerHT);

#endif