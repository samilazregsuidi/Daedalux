#ifndef INIT_STATE_H
#define INIT_STATE_H

class fsm;
class state;

// State
class initState {
public:
	static state* createInitState(const fsm* automata);
};

#endif