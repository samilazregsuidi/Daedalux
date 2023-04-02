#ifndef TRANSITION_VISITOR_H
#define TRANSITION_VISITOR_H

class transition;
class compTransition;
class programTransition;
class featProgTransition;
class processTransition;
class neverTransition;

class transitionVisitor {
public:
	virtual ~transitionVisitor() {};

	virtual void visit(transition* s) = 0;
	virtual void visit(compTransition* s) = 0;
	virtual void visit(programTransition* s) = 0;
	virtual void visit(featProgTransition* s) = 0;
	virtual void visit(processTransition* state) = 0;
	virtual void visit(neverTransition* s) = 0;
};

#endif
