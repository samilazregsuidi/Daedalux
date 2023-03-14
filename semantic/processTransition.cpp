#include "processTransition.hpp"

#include "fsmEdge.hpp"
#include "process.hpp"

#include <assert.h>
#include <iterator>

processTransition::processTransition(process* proc, const fsmEdge* edge) 
	: transition(proc)
	, edge(edge)
	//, features(ADD())
{
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

processTransition::~processTransition() {

}

/*void progTransition::fire(state* s) const {
	process* proc = this->getProc();
	assert(proc);
	//warning if "different" procs have the same pid i.e., dynamic proc creation
	proc = dynamic_cast<progState*>(s)->getProc(proc->getPid());

	proc->apply(this);
}*/

process* processTransition::getProc(void) const {
	return dynamic_cast<process*>(src);
}

const fsmEdge* processTransition::getEdge(void) const {
	return edge;
}

int processTransition::getLineNb(void) const {
	return getEdge()->getLineNb();
}