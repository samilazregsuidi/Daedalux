#ifndef PROGRAM_TRANSITION_H
#define PROGRAM_TRANSITION_H

/*
 * Execution of FSMs
 * * * * * * * * * * * * * * * * * * * * * * * */

#include "transition.hpp"


// ProcessTransitions are returned by the executables() function
class programTransition : public transition {
public:
	programTransition(state* s, transition* progTrans);

	programTransition(const programTransition* other);

	~programTransition() override;
	
	transition* getProgTrans(void) const;

	transition* deepCopy(void) const override;

	void accept(transitionVisitor* visitor) override;

	void print(void) const override;

public:		//
	transition* progTrans;
};

#endif