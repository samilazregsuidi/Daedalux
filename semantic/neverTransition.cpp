#include "neverTransition.hpp"
#include "transitionVisitor.hpp"

#include "state.hpp"
#include "never.hpp"
#include "programState.hpp"
#include "fsmEdge.hpp"
#include "process.hpp"

#include <assert.h>
#include <iterator>

neverTransition::neverTransition(never* proc, const fsmEdge* edge) 
	: transition(proc)
	, edge(edge)
	//, features(ADD())
{

	assert(proc);
	assert(edge);

	prob = edge->getProbability();
}

neverTransition::neverTransition(const neverTransition* other)
	: transition(other)
	, edge(other->edge)
{

}

transition* neverTransition::deepCopy(void) const {
	return new neverTransition(this);
}

neverTransition::~neverTransition() {

}

never* neverTransition::getNeverClaim(void) const {
	return dynamic_cast<never*>(src);
}

const fsmEdge* neverTransition::getEdge(void) const {
	return edge;
}

void neverTransition::accept(transitionVisitor* visitor) {
	visitor->visit(this);
}