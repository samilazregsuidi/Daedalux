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
class ptypeSymNode;
class neverSymNode;

#include "enumDef.hpp"
#include "mtypeVar.hpp"

class TVL;

typedef char byte;
typedef unsigned char ubyte;

// State
class initState {
public:
	static state* createInitState(const fsm* automata, const TVL* tvl = nullptr);

	static variable* createPrimitive(const std::string& name, const varSymNode* sym);

	static void addVariables(variable* v, const varSymNode* sym);

	static process* createProcess(const fsm* stateMachine, const ptypeSymNode* procType, unsigned int index);

	static never* createNever(const fsm* stateMachine, const neverSymNode* procType);
	
	static state* createProgState(const fsm* stateMachine, const std::string& name, const TVL* tvl, const sysSymNode* sym = nullptr);

	static transition* createProcTransition(process* proc, const fsmEdge* edge);

	static mtypeDef* createMtypeEnum(variable* v, const mtypedefSymNode* seq);

	static mtypeDef* mtype_def;
};

#endif