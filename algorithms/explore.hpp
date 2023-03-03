#ifndef EXPLORE_H
#define EXPLORE_H

#include <stack>

class fsm;
class symTable;
class state;
class TVL;

typedef char byte;
typedef unsigned char ubyte;

struct element {
	element(state* s)
		: s(s)
	{}

    ~element() {
        if(s) delete s;
        for(auto p : Post)
            delete p;
        for(auto p : Post_save)
            delete p;
    }

	state* s;
	std::list<state*> Post;
	std::list<state*> Post_save;
};

void launchExecution(const fsm* automata, const TVL* tvl = nullptr);
void createStateSpace(const fsm* automata, const TVL* tvl = nullptr);
void countStates(const fsm* automata, const TVL* tvl = nullptr);

void startNestedDFS(const fsm* automata, const TVL* tvl);
byte outerDFS(std::stack<element>& stackOuter);
byte innerDFS(std::stack<element>& stackOuter, std::stack<element>& stackInner);

#endif