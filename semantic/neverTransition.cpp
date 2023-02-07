#include "neverTransition.hpp"

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

/*neverTransition::neverTransition(state* s, process* proc, const fsmEdge* trans, const ADD& featExpr)
	: transition(s)
	, proc(proc)
	, edge(edge)
	//, features(featExpr)
{
	assert(proc && edge);
	prob = edge->getProbability();
}*/

neverTransition::~neverTransition() {

}

/*void progTransition::fire(state* s) const {
	process* proc = this->getProc();
	assert(proc);
	//warning if "different" procs have the same pid i.e., dynamic proc creation
	proc = dynamic_cast<progState*>(s)->getProc(proc->getPid());

	proc->apply(this);
}*/

never* neverTransition::getNeverClaim(void) const {
	return dynamic_cast<never*>(src);
}

const fsmEdge* neverTransition::getEdge(void) const {
	return edge;
}