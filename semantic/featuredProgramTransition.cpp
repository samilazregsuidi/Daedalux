#include "featuredProgramTransition.hpp"

#include "state.hpp"
#include "programState.hpp"
#include "fsmEdge.hpp"
#include "process.hpp"

#include <assert.h>
#include <iterator>

featProgTransition::featProgTransition(state* s, process* proc, const fsmEdge* edge, const ADD& featExpr) 
	: progTransition(s, proc, edge)
	, features(featExpr)
{
}

featProgTransition::~featProgTransition() {

}

const ADD& featProgTransition::getFeatExpr(void) const {
	return features;
}

/*void progTransition::fire(state* s) const {
	process* proc = this->getProc();
	assert(proc);
	//warning if "different" procs have the same pid i.e., dynamic proc creation
	proc = dynamic_cast<progState*>(s)->getProc(proc->getPid());

	proc->apply(this);
}*/