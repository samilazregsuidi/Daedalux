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

	programTransition(const programTransition* other);

	~programTransition() override;
	
	transition* getProcTrans(void) const;

	transition* getResponse(void) const;

	transition* deepCopy(void) const override;

	void accept(transitionVisitor* visitor) override;

public:		//
	transition* procTrans;
	transition* response;
};

#endif
