#include "compositeTransition.hpp"
#include "transition.hpp"
#include "transitionVisitor.hpp"

#include <cassert>
#include <iostream>
#include <algorithm>

compTransition::compTransition(state* s, const std::list<transition*>& Ts)
	: transition(s)
{
	add(Ts);
	for(auto t : Ts) {
		lines.merge(t->lines);
		prob *= t->prob;
		if(t->action != "") {
			if(action == "")
				action = t->action;
			else
				assert(action == t->action);
		}
	}
	assert(prob >= 0 && prob <= 1);
}

compTransition::compTransition(const compTransition* other) 
	: transition(other)
{
	for (auto t : other->subTs) {
		add(t->deepCopy());
	}
}

compTransition::~compTransition()
{
	auto copy = subTs;
	for (auto t : copy)
    	delete t;
}

transition* compTransition::deepCopy(void) const {
	return new compTransition(this);
}

std::list<transition *> compTransition::getSubTs(void) const
{
  return subTs;
}

void compTransition::accept(transitionVisitor* visitor) {
	visitor->visit(this);
}

void compTransition::print(void) const {
	std::cout << "Composite transition: ";
	for(auto t : subTs)
		t->print();
	std::cout << std::endl;
}