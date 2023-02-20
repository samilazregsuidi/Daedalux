#ifndef FEATURED_STATE_DECORATOR_H
#define FEATURED_STATE_DECORATOR_H

#include "stateDecorator.hpp"

#include "cuddObj.hh"

class TVL;

// State
class featStateDecorator : public stateDecorator {
public:

	featStateDecorator(state* wrappee, const ADD& diagram, const TVL* tvl); // Creates the initial state by setting all variables' value in the payload. Does not set the payloadHash.

	featStateDecorator(const featStateDecorator* other);

	state* deepCopy(void) const override;

	virtual ~featStateDecorator();

	std::list<transition*> executables(void) const override;

	// Applying statements

	state* apply(const transition* trans) override;

	void print(void) const override;

	const ADD& getFeatures(void) const;

	const ADD& getDiagram(void) const;

	bool constraint(const ADD& cst);

public:
	ADD features;
	const ADD diagram;
	const TVL* tvl;
};

#endif