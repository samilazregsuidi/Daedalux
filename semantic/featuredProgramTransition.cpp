#include "featuredProgramTransition.hpp"

#include "state.hpp"
#include "programState.hpp"
#include "fsmEdge.hpp"
#include "process.hpp"

#include "transitionVisitor.hpp"

#include <assert.h>
#include <iterator>
#include <iostream>

featProgTransition::featProgTransition(state* s, transition* procTrans, ADD featExpr, transition* response)
	: programTransition(s, procTrans, response)
	, features(featExpr)
{
}

featProgTransition::featProgTransition(const featProgTransition* other) 
	: programTransition(other)
	, features(other->features)
{
}

featProgTransition::~featProgTransition() 
{
}

ADD featProgTransition::getFeatExpr(void) const {
	return features;
}

transition* featProgTransition::deepCopy(void) const {
	return new featProgTransition(this);
}

void featProgTransition::accept(transitionVisitor* visitor) {
	visitor->visit(this);
}