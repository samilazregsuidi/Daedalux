#ifndef STATE_DECORATOR_H
#define STATE_DECORATOR_H

#include "state.hpp"

// State
class stateDecorator : public state {
public:

	stateDecorator(state* wrappee) 
		: state(wrappee)
		, wrappee(wrappee)
	{}

	stateDecorator(const stateDecorator& s) 
		: state(s)
		, wrappee(s.wrappee)
	{}

	stateDecorator(const stateDecorator* other)
		: state(other)
		, wrappee(other->wrappee)
	{}

	virtual state* deepCopy(void) const = 0;

	virtual ~stateDecorator() {
		delete wrappee;
	}

	virtual std::list<transition*> executables(void) const = 0;

	virtual state* apply(const transition* trans) = 0;

	virtual bool nullstate(void) const {
		return wrappee->nullstate();
	}

	virtual bool endstate(void) const {
		return wrappee->endstate();
	}

	virtual bool isAccepting(void) const {
		return wrappee->isAccepting();
	}

	virtual state* getNeverClaim(void) const {
		return wrappee->getNeverClaim();
	}

public:
	state* wrappee;
};

#endif