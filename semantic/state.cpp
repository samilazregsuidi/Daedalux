#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <cmath>

#include "state.hpp"
#include "process.hpp"

#include "state.hpp"
/**
 * Adds the global variables in the memory chunk.
 *
 * Does not set the payloadHash.
 */

state::state(variable::Type type, const std::string& name)
	: variable(type, name)
	, prob(1.0)
	, trans(nullptr)
{
}

state::state(const state& other)
	: variable(other)
	, prob(other.prob)
	, trans(other.trans)
{
}

state::state(const state* other)
	: variable(other)
	, prob(other->prob)
	, trans(other->trans)
{
}

state::~state() {}

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
	assert(copy);
	assert(s->hash() == copy->hash());
	printf("s hash %lu and copy hash %lu\n", s->hash(), copy->hash());
	copy->apply(t);
	return copy;
}

