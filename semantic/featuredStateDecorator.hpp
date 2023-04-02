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

	~featStateDecorator() override;

	std::list<transition*> executables(void) const override;

	byte compare(const state& s2) const override;

	byte compare(const state& s2, const ADD& features) const;

	byte compare(unsigned long s2Hash, const ADD& features) const;

	// Applying statements

	state* apply(transition* trans) override;

	void print(void) const override;

	ADD getFeatures(void) const;

	ADD getDiagram(void) const;

	bool constraint(const ADD& cst);

	void accept(stateVisitor* visitor) override;

public:
	ADD features;
	ADD diagram;
	const TVL* tvl;
};

#endif