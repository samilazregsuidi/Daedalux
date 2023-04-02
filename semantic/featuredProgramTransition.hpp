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
	featProgTransition(state* s, transition* procTrans, ADD featExpr, transition* response = nullptr);

	featProgTransition(const featProgTransition* other);
	
	~featProgTransition() override;

	ADD getFeatExpr(void) const;

	transition* deepCopy(void) const override;

	void accept(transitionVisitor* visitor) override;

public:		//
	ADD features;
};

#endif
