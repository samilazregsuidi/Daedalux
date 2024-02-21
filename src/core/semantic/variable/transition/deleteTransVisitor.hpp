#ifndef DELETE_TRANSITION_VISITOR_H
#define DELETE_TRANSITION_VISITOR_H

#include "transitionVisitor.hpp"

#include <set>

class delTransitionVisitor : public transitionVisitor {
public:
	~delTransitionVisitor() override {};
	void deleteVisited(void);
	void visit(transition* t) override;
	void visit(compTransition* t) override;
	void visit(rendezVousTransition* t) override;
	void visit(featTransition* t) override;
	void visit(threadTransition* t) override;

	std::set<transition*> table;
};

#endif
