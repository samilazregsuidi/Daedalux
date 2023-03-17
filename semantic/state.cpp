#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <cmath>

#include "state.hpp"
#include "process.hpp"

#include "state.hpp"

#include "stateVisitor.hpp"

/**
 * Adds the global variables in the memory chunk.
 *
 * Does not set the payloadHash.
 */


state::state(variable::Type type, const std::string& name)
	: variable(type, name)
	, prob(1.0)
	, origin(nullptr)
	, errorMask(0)
{
}

state::state(const state& other)
	: variable(other)
	, prob(other.prob)
	, origin(other.origin)
	, errorMask(other.errorMask)
{}

state::state(const state* other)
	: variable(other)
	, prob(other->prob)
	, origin(other->origin)
	, errorMask(other->errorMask)
{}

state::~state() {
	if(origin)
		delete origin;
}

void state::addError(unsigned int e) {
	errorMask |= e;
}

unsigned int state::getErrorMask(void) const {
	return errorMask;
}

bool state::hasDeadlock(void) const {
	return executables().size() == 0;
}

std::list<state*> state::Post(void) const {
	std::list<state*> res;
	for(auto t : executables())
		res.push_back(this->Post(t));
	return res;

}

state* state::Post(const transition* trans) const {
	return state::apply(this, trans);
}

/*static*/ state* state::applyRepeated(const std::list<transition*>& transList) {
	for (auto t : transList)
		this->apply(t);
	return this;
}

/*static*/ state* state::apply(const state* s, const transition* t) {
	auto copy = s->deepCopy();
	//printf("copy print\n");
	//copy->print();
	assert(copy);
	assert(s->hash() == copy->hash());
	//printf("s hash %lu and copy hash %lu\n", s->hash(), copy->hash());
	copy->apply(t);
	return copy;
}

const transition* state::getOrigin(void) const {
	return origin;
}

double state::getProbability(void) const {
	return prob;
}

byte state::compare(const state& s2) const {
	return hash() == s2.hash();
}

void state::accept(stateVisitor* visitor) {
	visitor->visit(this);
}