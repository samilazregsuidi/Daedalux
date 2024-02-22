#ifndef COMPOSITE_TRANSITION_H
#define COMPOSITE_TRANSITION_H

/*
 * Execution of FSMs
 * * * * * * * * * * * * * * * * * * * * * * * */

#include <list>
#include <vector>

#include "transition.hpp"

class state;

// ProcessTransitions are returned by the executables() function
class compTransition : public transition {
public:
	compTransition(state* s, const std::list<transition*>& ts);

	compTransition(const compTransition* other);
	
	~compTransition() override;
	
	transition* deepCopy(void) const override;

	void accept(transitionVisitor* visitor) override;

	//state* fire(void) override;

	std::list<transition*> getSubTs(void) const;

	void print(void) const override;
};

#endif
