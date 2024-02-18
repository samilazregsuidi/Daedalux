#include "progTransition.hpp"
#include "transitionVisitor.hpp"
#include "threadTransition.hpp"
#include "programState.hpp"

#include <assert.h>
#include <iterator>
#include <iostream>

progTransition::progTransition(program* s, threadTransition* procTrans) 
	: transition(s)
	, procTrans(procTrans)
{
	assert(s);
	assert(procTrans);

	prob = procTrans->getProbability();
	assert(prob >= 0 && prob <= 1);

	//lines.push_back(dynamic_cast<processTransition*>(procTrans)->getLineNb());

	action = procTrans->action;

}

progTransition::progTransition(const progTransition* other)
	: transition(other)
	, procTrans(dynamic_cast<threadTransition*>(other->procTrans->deepCopy()))
{
}

progTransition::~progTransition() {
	delete procTrans;
}

threadTransition* progTransition::getProcTrans(void) const {
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

float progTransition::similarity(const transition* other) const {
	auto cast = dynamic_cast<const progTransition*>(other);
	return procTrans->similarity(cast->procTrans);
}

void progTransition::print(void) const {
	std::cout << "progTransition: ";
	procTrans->print();
	std::cout << std::endl;
}