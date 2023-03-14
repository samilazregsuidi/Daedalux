#ifndef PROCESS_TRANSITION_H
#define PROCESS_TRANSITION_H

/*
 * Execution of FSMs
 * * * * * * * * * * * * * * * * * * * * * * * */

#include "transition.hpp"

//#include "cuddObj.hh"

class fsmEdge;
class process;

// ProcessTransitions are returned by the executables() function
class processTransition : public transition {
public:
	processTransition(process* proc, const fsmEdge* trans);

	//progTransition(state* s, process* proc, const fsmEdge* trans, const ADD& featExpr);
	
	~processTransition() override;
	
	process* getProc(void) const;

	const fsmEdge* getEdge(void) const;

	int getLineNb(void) const;

	//void fire(state* s) const override;

public:		//
	const fsmEdge* const edge;			//  - The transition that can be fired
};

#endif
