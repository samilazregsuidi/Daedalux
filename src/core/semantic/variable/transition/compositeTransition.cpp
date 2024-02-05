#include "compositeTransition.hpp"
#include "transition.hpp"
#include "transitionVisitor.hpp"
#include "state.hpp"

#include <iostream>

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

bool compTransition::operator==(const transition* other) const {
	if(!other)
		return false;

	for(auto t : subTs){
		bool found = false;
		for(auto o : other->subTs){
			if(*t == o){
				found = true;
				break;
			}
		}
		if(!found)
			return false;
	}
	return true;
}