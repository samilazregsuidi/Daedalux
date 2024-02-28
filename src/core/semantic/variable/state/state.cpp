#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <cmath>

#include "state.hpp"
#include "process.hpp"

#include "state.hpp"

#include "stateVisitor.hpp"
#include <iostream>

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
	, origin(nullptr)
	, errorMask(other.errorMask)
{}

state::state(const state* other)
	: variable(other)
	, prob(other->prob)
	, origin(nullptr)
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
		res.push_back(fire(t));
	
	//check if it is a deadlock state
	auto neverClaim = getNeverClaim();
	if(neverClaim) {
		auto neverTs = neverClaim->executables();
		if(res.empty() && !neverTs.empty()){
			std::cout << "Deadlock detected because of never claim" << std::endl;
			this->errorMask |= ERR_DEADLOCK;
		}
		transition::erase(neverTs);
	}
	return res;
}

state* state::fire(transition* trans) const {
	assert(trans->src == this);
	assert(trans->dst == nullptr);

	auto copy = deepCopy();
	assert(copy);
	assert(this != copy);

	assert(hash() == copy->hash()); // The copy should have the same hash as the original
	assert(copy->getOrigin() == nullptr); //the origin has been reset to nullptr

	//this->print();

	copy->apply(trans);

	//copy->print();

	//trans->print();

	assert(copy->origin == trans);
	assert(trans->dst == copy);

	return copy;
}

/*state* state::Post(transition* trans) const {
	auto post = state::apply(this, trans);
	assert(trans->src == this);
	trans->dst = post;
	return post;
}*/

void state::applyRepeated(const std::list<transition*>& transList) {
	for (auto t : transList)
		this->apply(t);
}

// state* state::apply(const state* s, transition* t) {
// 	auto copy = s->deepCopy();
// 	assert(copy->getOrigin() == nullptr);
// 	//printf("copy print\n");
// 	//copy->print();
// 	// Apply the transition to the copy
// 	assert(copy);
// 	assert(s->hash() == copy->hash()); // The copy should have the same hash as the original
// 	copy->apply(t);
// 	assert(copy->getOrigin() == t);
// 	return copy;
// }

const transition* state::getOrigin(void) const {
	return origin;
}

double state::getProbability(void) const {
	return prob;
}

byte state::compare(const state& s2) const {
	return hash() == s2.hash();
}

byte state::compare(unsigned long s2Hash) const {
	return hash() == s2Hash;
}

void state::accept(stateVisitor* visitor) {
	visitor->visit(this);
}