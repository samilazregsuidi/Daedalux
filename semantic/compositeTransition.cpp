#include "compositeTransition.hpp"
#include "transition.hpp"

#include "state.hpp"

compTransition::compTransition(state* s, const std::list<transition*>& Ts)
	: transition(s)
	, Ts(Ts)
{}

compTransition::compTransition(state* s, const std::vector<transition*>& Ts)
	: transition(s)
	, Ts(Ts.begin(), Ts.end())
{}

compTransition::~compTransition(){
	
}

/*void compTransition::fire(state* s) const {
	for(auto trans : Ts) {
		trans->fire();
	}
}*/
