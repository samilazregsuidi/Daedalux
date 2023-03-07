#ifndef STATE_VISITOR_H
#define STATE_VISITOR_H

class state;
class process;
class progState;
class compState;
class featStateDecorator;

class stateVisitor {
public:
	virtual void visit(state* s) = 0;
	virtual void visit(process* s) = 0;
	virtual void visit(progState* s) = 0;
	virtual void visit(compState* s) = 0;
	virtual void visit(featStateDecorator* s) = 0;
};

#endif
