#include "compositeTransition.hpp"
#include "transition.hpp"
#include "transitionVisitor.hpp"
#include "state.hpp"

#include <iostream>

compTransition::compTransition(state* s, const std::list<transition*>& Ts)
	: transition(s)
{
	add(Ts);
	for(auto t : Ts)
		lines.merge(t->lines);
}

compTransition::compTransition(const compTransition* other) 
	: transition(other)
{
}

compTransition::~compTransition()
{
}

transition* compTransition::deepCopy(void) const {
	return new compTransition(this);
}

void compTransition::accept(transitionVisitor* visitor) {
	visitor->visit(this);
}