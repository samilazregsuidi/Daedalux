#include "transition.hpp"

#include <assert.h>
#include <iterator>

transition* transition::sample(const std::list<transition*>& transList) {
	if(transList.size() == 0)
		return nullptr;
	auto it = transList.begin();
	std::advance(it, rand() % transList.size());
	return *it;
}

transition::transition(state* s) 
	: src(s)
{
	assert(s);
}

transition::~transition(){
	for(auto r : responses)
		delete r;
}

std::list<transition*> transition::getResponses(void) const {
	return responses;
}