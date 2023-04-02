#include "processTransition.hpp"
#include "transitionVisitor.hpp"

#include "fsmEdge.hpp"
#include "process.hpp"

#include <assert.h>
#include <iterator>
#include <iostream>

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

processTransition::processTransition(const processTransition* other)
	: transition(other)
	, edge(other->edge)
{
}

processTransition::~processTransition() 
{
}

process* processTransition::getProc(void) const {
	return dynamic_cast<process*>(src);
}

const fsmEdge* processTransition::getEdge(void) const {
	return edge;
}

int processTransition::getLineNb(void) const {
	return getEdge()->getLineNb();
}

transition* processTransition::deepCopy(void) const {
	return new processTransition(this);
}

void processTransition::accept(transitionVisitor* visitor) {
	visitor->visit(this);
}