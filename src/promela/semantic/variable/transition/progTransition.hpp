#ifndef PROGRAM_TRANSITION_H
#define PROGRAM_TRANSITION_H

/*
 * Execution of FSMs
 * * * * * * * * * * * * * * * * * * * * * * * */

#include "transition.hpp"

class threadTransition;
class program;

// ProcessTransitions are returned by the executables() function
class progTransition : public transition {
public:
	progTransition(program* s, threadTransition* procTrans);

	progTransition(const progTransition* other);

	~progTransition() override;
	
	threadTransition* getProcTrans(void) const;

	transition* deepCopy(void) const override;

	void accept(transitionVisitor* visitor) override;

	bool operator==(const transition* other) const override;

	float similarity(const transition* other) const override;

	void print(void) const override;

public:		//
	threadTransition* procTrans;
};

#endif
