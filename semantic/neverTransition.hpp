#ifndef NEVER_TRANSITION_H
#define NEVER_TRANSITION_H

/*
 * Execution of FSMs
 * * * * * * * * * * * * * * * * * * * * * * * */

#include "transition.hpp"

//#include "cuddObj.hh"

class fsmEdge;
class never;

// ProcessTransitions are returned by the executables() function
class neverTransition : public transition {
public:
	neverTransition(never* n, const fsmEdge* trans);

	//progTransition(state* s, process* proc, const fsmEdge* trans, const ADD& featExpr);
	
	~neverTransition() override;
	
	never* getNeverClaim(void) const;

	const fsmEdge* getEdge(void) const;

	//void fire(state* s) const override;

public:		//
	const fsmEdge* const edge;			//  - The transition that can be fired
	//ADD features;
};

#endif
