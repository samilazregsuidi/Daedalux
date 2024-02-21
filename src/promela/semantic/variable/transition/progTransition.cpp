#include "progTransition.hpp"
#include "transitionVisitor.hpp"

#include "fsmEdge.hpp"
#include "process.hpp"

#include <assert.h>
#include <iterator>
#include <iostream>

programTransition::programTransition(state* s, transition* progTrans) 
	: transition(s)
	, progTrans(progTrans)
{
	assert(s);
	assert(progTrans);

	add(progTrans);

	prob = progTrans->getProbability();

	lines.merge(progTrans->lines);

	action = progTrans->action;

}

programTransition::programTransition(const programTransition* other)
	: transition(other)
	, progTrans(nullptr)
{
	auto it = subTs.begin();
	progTrans = *it;
}

programTransition::~programTransition() {
}

transition* programTransition::getProgTrans(void) const {
	return progTrans;
}

transition* programTransition::deepCopy(void) const {
	return new programTransition(this);
}

void programTransition::accept(transitionVisitor* visitor) {
	visitor->visit(this);
}

void programTransition::print(void) const {
	std::cout << "Program transition: ";
	progTrans->print();
}