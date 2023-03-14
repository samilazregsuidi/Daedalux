#ifndef FEATURED_PROGRAM_TRANSITION_H
#define FEATURED_PROGRAM_TRANSITION_H

/*
 * Execution of FSMs
 * * * * * * * * * * * * * * * * * * * * * * * */

#include "programTransition.hpp"

#include "cuddObj.hh"


// ProcessTransitions are returned by the executables() function
class featProgTransition : public programTransition {
public:
	featProgTransition(state* s, transition* procTrans, const ADD& featExpr, transition* response = nullptr);
	
	~featProgTransition() override;

	const ADD& getFeatExpr(void) const;

public:		//
	ADD features;
};

#endif
