#include "progTransition.hpp"
#include "transitionVisitor.hpp"

#include "fsmEdge.hpp"
#include "process.hpp"

#include <assert.h>
#include <iterator>
#include <iostream>

programTransition::programTransition(state* s, transition* procTrans) 
	: transition(s)
	, procTrans(procTrans)
{
	assert(s);
	assert(procTrans);

	add(procTrans);

	prob = procTrans->getProbability();

	lines.merge(procTrans->lines);

	action = procTrans->action;

}

programTransition::programTransition(const programTransition* other)
	: transition(other)
	, procTrans(nullptr)
{
	auto it = subTs.begin();
	procTrans = *it;
}

programTransition::~programTransition() {
}

transition* programTransition::getProcTrans(void) const {
	return procTrans;
}

transition* programTransition::deepCopy(void) const {
	return new programTransition(this);
}

void programTransition::accept(transitionVisitor* visitor) {
	visitor->visit(this);
}

void programTransition::print(void) const {
	std::cout << "Program transition: ";
	procTrans->print();
}