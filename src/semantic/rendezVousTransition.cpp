#include "rendezVousTransition.hpp"
#include "processTransition.hpp"
#include "transitionVisitor.hpp"

#include "fsmEdge.hpp"
#include "process.hpp"

#include <assert.h>
#include <iterator>
#include <iostream>

rendezVousTransition::rendezVousTransition(state* s, transition* procTrans, transition* response) 
	: transition(s)
	, procTrans(procTrans)
	, response(response)
{
	assert(s);
	assert(procTrans);

	add(procTrans);
	add(response);

	prob = procTrans->getProbability() * (response ? response->getProbability() : 1.0);
	assert(prob >= 0 && prob <= 1);

	lines.push_back(dynamic_cast<processTransition*>(procTrans)->getLineNb());
	if(response)
		lines.push_back(dynamic_cast<processTransition*>(response)->getLineNb());

	action = procTrans->action;

}

rendezVousTransition::rendezVousTransition(const rendezVousTransition* other)
	: transition(other)
	, procTrans(nullptr)
	, response(nullptr)
{
	auto it = subTs.begin();
	procTrans = *it;
	if(++it != subTs.end())
		response = *it;
}

rendezVousTransition::~rendezVousTransition() {
}

transition* rendezVousTransition::getProcTrans(void) const {
	return procTrans;
}

transition* rendezVousTransition::getResponse(void) const {
	return response;
}

transition* rendezVousTransition::deepCopy(void) const {
	return new rendezVousTransition(this);
}

void rendezVousTransition::accept(transitionVisitor* visitor) {
	visitor->visit(this);
}

bool rendezVousTransition::operator==(const transition* other) const {
	auto cast = dynamic_cast<const rendezVousTransition*>(other);
	return *procTrans == cast->procTrans && *response == cast->response;
}