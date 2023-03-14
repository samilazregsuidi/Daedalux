#ifndef PROGRAM_TRANSITION_H
#define PROGRAM_TRANSITION_H

/*
 * Execution of FSMs
 * * * * * * * * * * * * * * * * * * * * * * * */

#include "transition.hpp"

//#include "cuddObj.hh"

// ProcessTransitions are returned by the executables() function
class programTransition : public transition {
public:
	programTransition(state* s, transition* procTrans, transition* response = nullptr);

	//progTransition(state* s, process* proc, const fsmEdge* trans, const ADD& featExpr);
	
	~programTransition() override;
	
	transition* getProcTrans(void) const;

	transition* getResponse(void) const;

public:		//
	transition* procTrans;
	transition* response;
};

#endif
