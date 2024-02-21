#ifndef INIT_STATE_H
#define INIT_STATE_H

#include <list>
#include <string>

class fsm;
class fsmEdge;

class state;
class process;
class never;
class program;
class transition;
class variable;
class varSymNode;
class seqSymNode;
class sysSymNode;

class TVL;

typedef char byte;
typedef unsigned char ubyte;

// State
class initState {
public:
	static state* createInitState(const fsm* automata, const TVL* tvl);

	static std::list<variable*> addVariables(variable* v, const varSymNode* sym);

	static process* createProcess(const fsm* stateMachine, const seqSymNode* procType, byte pid, unsigned int index);

	static never* createNever(const fsm* stateMachine, const seqSymNode* procType);
	
	static state* createProgState(const fsm* stateMachine, const std::string& name, const TVL* tvl, const sysSymNode* sym = nullptr);

	static transition* createTransition(const fsmEdge* edge, program* s, process* proc, transition* response = nullptr);
};

#endif