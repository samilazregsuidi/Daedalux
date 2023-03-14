#include "programTransition.hpp"
#include "processTransition.hpp"

#include "fsmEdge.hpp"
#include "process.hpp"

#include <assert.h>
#include <iterator>

programTransition::programTransition(state* s, transition* procTrans, transition* response) 
	: transition(s)
	, procTrans(procTrans)
	, response(response)
	//, features(ADD())
{
	assert(s);
	assert(procTrans);

	prob = procTrans->getProbability() * (response ? response->getProbability() : 1.0);

	lines.push_back(dynamic_cast<processTransition*>(procTrans)->getLineNb());
	if(response)
		lines.push_back(dynamic_cast<processTransition*>(response)->getLineNb());

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

programTransition::~programTransition() {

}

/*void progTransition::fire(state* s) const {
	process* proc = this->getProc();
	assert(proc);
	//warning if "different" procs have the same pid i.e., dynamic proc creation
	proc = dynamic_cast<progState*>(s)->getProc(proc->getPid());

	proc->apply(this);
}*/

transition* programTransition::getProcTrans(void) const {
	return procTrans;
}

transition* programTransition::getResponse(void) const {
	return response;
}