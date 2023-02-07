#ifndef FEATURED_STATE_DECORATOR_H
#define FEATURED_STATE_DECORATOR_H

#include "stateDecorator.hpp"

#include "cuddObj.hh"

// State
class featStateDecorator : public stateDecorator {
public:

	featStateDecorator(state* wrappee, const ADD diagram); // Creates the initial state by setting all variables' value in the payload. Does not set the payloadHash.

	featStateDecorator(const featStateDecorator* other);

	state* deepCopy(void) const override;

	virtual ~featStateDecorator();

	std::list<transition*> executables(void) const override;

	// Applying statements

	state* apply(const transition* trans) override;

	void print(void) const override;

	const ADD& getFeatures(void) const;

	const ADD& getDiagram(void) const;

public:
	fsm* stateMachine;
	ADD features;
	const ADD diagram;
};

#endif