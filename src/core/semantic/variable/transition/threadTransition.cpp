#include "threadTransition.hpp"
#include "transitionVisitor.hpp"

#include "fsmEdge.hpp"
#include "thread.hpp"

#include "flowStmnt.hpp"

#include <assert.h>
#include <iterator>
#include <iostream>

threadTransition::threadTransition(thread* proc, const fsmEdge* edge) 
	: transition(proc)
	, edge(edge)
	//, features(ADD())
{
	assert(proc);
	assert(edge);

	prob = edge->getProbability();
	assert(prob >= 0 && prob <= 1);

	//lines.push_back(edge->getLineNb());

	auto expression = edge->getExpression();
	if(expression->getType() == astNode::E_STMNT_ACTION) {
		action = dynamic_cast<const stmntAction*>(expression)->getLabel();
	}

}

threadTransition::threadTransition(const threadTransition* other)
	: transition(other)
	, edge(other->edge)
{
}

threadTransition::~threadTransition() 
{
}

thread* threadTransition::getThread(void) const {
	return dynamic_cast<thread*>(src);
}

const fsmEdge* threadTransition::getEdge(void) const {
	return edge;
}

unsigned int threadTransition::getLine(void) const {
	return edge->getLineNb();
}

int threadTransition::getLineNb(void) const {
	return getEdge()->getLineNb();
}

astNode::Type threadTransition::getType(void) const {
	auto type = edge->getExpression()->getType();
	if(type == astNode::E_STMNT_EXPR){
		auto expr = dynamic_cast<const stmntExpr*>(edge->getExpression())->getChild();
		return expr->getType();
	} else {
		return type;
	}
}

transition* threadTransition::deepCopy(void) const {
	return new threadTransition(this);
}

void threadTransition::accept(transitionVisitor* visitor) {
	visitor->visit(this);
}

bool threadTransition::operator==(const transition* other) const {
	auto cast = dynamic_cast<const threadTransition*>(other);
	return cast && *edge == *cast->edge;
}

float threadTransition::similarity(const transition* other) const {
	auto cast = dynamic_cast<const threadTransition*>(other);
	return cast ? edge->similarity(*cast->edge) : 0;
}

void threadTransition::print(void) const {
	std::cout << "Thread transition: " << std::string(*edge->getExpression()) << " @ line " <<  edge->getLineNb() <<std::endl;
}