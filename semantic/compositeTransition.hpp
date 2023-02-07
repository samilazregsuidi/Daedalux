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

	compTransition(state* s, const std::vector<transition*>& ts);
	
	~compTransition() override;
	
	//void fire(state* s) const override;

public:		//
	std::list<transition*> Ts;
};

#endif
