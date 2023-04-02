#include "deleteTransVisitor.hpp"

#include "transition.hpp"
#include "compositeTransition.hpp"
#include "programTransition.hpp"
#include "featuredProgramTransition.hpp"
#include "processTransition.hpp"
#include "neverTransition.hpp"

#include <assert.h>

void delTransitionVisitor::deleteVisited(void) {
	for(auto t : table) {
		t->detach();
		delete t;
	}
}

void delTransitionVisitor::visit(transition* t) {
	assert(false);
}

void delTransitionVisitor::visit(compTransition* t) {
	for(auto sT : t->subTs)
		sT->accept(this);
	table.insert(t);
}

void delTransitionVisitor::visit(programTransition* t) {
	t->procTrans->accept(this);
	if(t->response)
		t->response->accept(this);
	table.insert(t);
}

void delTransitionVisitor::visit(featProgTransition* t) {
	t->procTrans->accept(this);
	if(t->response)
		t->response->accept(this);
	table.insert(t);
}

void delTransitionVisitor::visit(processTransition* t) {
	table.insert(t);
}

void delTransitionVisitor::visit(neverTransition* t) {
	table.insert(t);
}