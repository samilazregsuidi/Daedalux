#include "programTransition.hpp"
#include "processTransition.hpp"
#include "transitionVisitor.hpp"

#include "fsmEdge.hpp"
#include "process.hpp"

#include <assert.h>
#include <iterator>
#include <iostream>

programTransition::programTransition(state* s, transition* procTrans, transition* response) 
	: transition(s)
	, procTrans(procTrans)
	, response(response)
{
	assert(s);
	assert(procTrans);

	add(procTrans);
	add(response);

	prob = procTrans->getProbability() * (response ? response->getProbability() : 1.0);

	lines.push_back(dynamic_cast<processTransition*>(procTrans)->getLineNb());
	if(response)
		lines.push_back(dynamic_cast<processTransition*>(response)->getLineNb());

}

programTransition::programTransition(const programTransition* other)
	: transition(other)
	, procTrans(nullptr)
	, response(nullptr)
{
	auto it = subTs.begin();
	procTrans = *it;
	if(++it != subTs.end())
		response = *it;
}

programTransition::~programTransition() {
}

transition* programTransition::getProcTrans(void) const {
	return procTrans;
}

transition* programTransition::getResponse(void) const {
	return response;
}

transition* programTransition::deepCopy(void) const {
	return new programTransition(this);
}

void programTransition::accept(transitionVisitor* visitor) {
	visitor->visit(this);
}