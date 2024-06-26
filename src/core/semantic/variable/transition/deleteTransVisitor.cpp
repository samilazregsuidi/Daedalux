#include "deleteTransVisitor.hpp"

#include "transition.hpp"
#include "compositeTransition.hpp"
#include "rendezVousTransition.hpp"
#include "featuredTransition.hpp"
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

void delTransitionVisitor::visit(rendezVousTransition* t) {
	t->question->accept(this);
	if(t->response)
		t->response->accept(this);
	table.insert(t);
}

void delTransitionVisitor::visit(featTransition* t) {
	t->wrappee->accept(this);
	table.insert(t);
}

void delTransitionVisitor::visit(processTransition* t) {
	table.insert(t);
}

void delTransitionVisitor::visit(neverTransition* t) {
	table.insert(t);
}