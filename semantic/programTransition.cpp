#include "programTransition.hpp"

#include "state.hpp"
#include "programState.hpp"
#include "fsmEdge.hpp"
#include "process.hpp"

#include <assert.h>
#include <iterator>

progTransition::progTransition(state* s, process* proc, const fsmEdge* edge) 
	: transition(s)
	, proc(proc)
	, edge(edge)
	//, features(ADD())
{
	assert(s);
	assert(proc);
	assert(edge);

	prob = edge->getProbability();

	lines.push_back(edge->getLineNb());
}

/*progTransition::progTransition(state* s, process* proc, const fsmEdge* trans, const ADD& featExpr)
	: transition(s)
	, proc(proc)
	, edge(edge)
	//, features(featExpr)
{
	assert(proc && edge);
	prob = edge->getProbability();
}*/

progTransition::~progTransition() {

}

/*void progTransition::fire(state* s) const {
	process* proc = this->getProc();
	assert(proc);
	//warning if "different" procs have the same pid i.e., dynamic proc creation
	proc = dynamic_cast<progState*>(s)->getProc(proc->getPid());

	proc->apply(this);
}*/

process* progTransition::getProc(void) const {
	return proc;
}

const fsmEdge* progTransition::getEdge(void) const {
	return edge;
}