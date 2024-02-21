#include "featuredTransition.hpp"

#include "state.hpp"
#include "fsmEdge.hpp"
#include "process.hpp"

#include "transitionVisitor.hpp"

#include <assert.h>
#include <iterator>
#include <iostream>

featTransition::featTransition(state* s, transition* wrappee, ADD featExpr)
	: transition(s)
	, features(featExpr)
	, wrappee(wrappee)
{
	assert(wrappee);
}

featTransition::featTransition(const featTransition* other) 
	: transition(other)
	, features(other->features)
	, wrappee(other->wrappee->deepCopy())
{
}

featTransition::~featTransition() 
{
	delete wrappee;
}

ADD featTransition::getFeatExpr(void) const {
	return features;
}

transition* featTransition::deepCopy(void) const {
	return new featTransition(this);
}

void featTransition::accept(transitionVisitor* visitor) {
	visitor->visit(this);
}

bool featTransition::operator==(const transition* other) const {
	auto cast = dynamic_cast<const featTransition*>(other);
	return features == cast->features && *wrappee == cast->wrappee;
}

float featTransition::similarity(const transition* other) const {
	auto cast = dynamic_cast<const featTransition*>(other);
	return features == cast->features ? wrappee->similarity(cast->wrappee) : 0;
}

void featTransition::print(void) const {
	std::cout << "featTransition: ";
	wrappee->print();
	std::cout << " with features: " << features << std::endl;
}