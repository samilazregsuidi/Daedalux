#include <daedalux/core/semantic/variable/transition/visitor/deleteTransVisitor.hpp>

#include <daedalux/core/semantic/variable/transition.hpp>
#include <daedalux/promela/semantic/variable/transition.hpp>
#include <daedalux/feature/semantic/variable/transition.hpp>

#include <assert.h>

void delTransitionVisitor::deleteVisited(void) {
	for(auto t : table) {
		//t->detach();
		delete t;
	}
}

void delTransitionVisitor::visit(transition* t) {
	assert(false);
}

void delTransitionVisitor::visit(compTransition* t) {
	for(auto sT : t->getSubTs())
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

void delTransitionVisitor::visit(threadTransition* t) {
	table.insert(t);
}