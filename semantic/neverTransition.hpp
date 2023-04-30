#ifndef NEVER_TRANSITION_H
#define NEVER_TRANSITION_H

/*
 * Execution of FSMs
 * * * * * * * * * * * * * * * * * * * * * * * */
#include "never.hpp"
#include "transition.hpp"
#include "fsmEdge.hpp"

//#include "cuddObj.hh"

// ProcessTransitions are returned by the executables() function
class neverTransition : public transition {
public:
	neverTransition(never* n, const fsmEdge* trans);

	neverTransition(const neverTransition* other);
	
	~neverTransition() override;
	
	never* getNeverClaim(void) const;

	const fsmEdge* getEdge(void) const;

	transition* deepCopy(void) const override;

	void accept(transitionVisitor* visitor) override;

public:		//
	const fsmEdge* const edge;			//  - The transition that can be fired
	//ADD features
};

#endif
