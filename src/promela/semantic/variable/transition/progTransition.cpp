#include "progTransition.hpp"
#include "processTransition.hpp"
#include "transitionVisitor.hpp"

#include <assert.h>
#include <iterator>
#include <iostream>

progTransition::progTransition(state* s, transition* procTrans) 
	: transition(s)
	, procTrans(procTrans)
{
	assert(s);
	assert(procTrans);

	add(procTrans);

	prob = procTrans->getProbability();
	assert(prob >= 0 && prob <= 1);

	lines.push_back(dynamic_cast<processTransition*>(procTrans)->getLineNb());

	action = procTrans->action;

}

progTransition::progTransition(const progTransition* other)
	: transition(other)
	, procTrans(other->procTrans)
{
	auto it = subTs.begin();
	procTrans = *it;
	if(++it != subTs.end())
		procTrans = *it;
}

progTransition::~progTransition() {
}

transition* progTransition::getProcTrans(void) const {
	return procTrans;
}

transition* progTransition::deepCopy(void) const {
	return new progTransition(this);
}

void progTransition::accept(transitionVisitor* visitor) {
	visitor->visit(this);
}

bool progTransition::operator==(const transition* other) const {
	auto cast = dynamic_cast<const progTransition*>(other);
	return *procTrans == cast->procTrans;
}