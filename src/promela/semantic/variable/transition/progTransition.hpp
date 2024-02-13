#ifndef PROGRAM_TRANSITION_H
#define PROGRAM_TRANSITION_H

/*
 * Execution of FSMs
 * * * * * * * * * * * * * * * * * * * * * * * */

#include "transition.hpp"

// ProcessTransitions are returned by the executables() function
class progTransition : public transition {
public:
	progTransition(state* s, transition* procTrans);

	progTransition(const progTransition* other);

	~progTransition() override;
	
	transition* getProcTrans(void) const;

	transition* deepCopy(void) const override;

	void accept(transitionVisitor* visitor) override;

	bool operator==(const transition* other) const override;

public:		//
	transition* procTrans;
};

#endif
