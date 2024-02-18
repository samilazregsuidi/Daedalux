#ifndef STATE_VISITOR_H
#define STATE_VISITOR_H

class state;
class process;
class program;
class never;
class compState;
class featStateDecorator;

class stateVisitor {
public:
	virtual ~stateVisitor() {};

	virtual void visit(state* s) = 0;
	virtual void visit(process* s) = 0;
	virtual void visit(program* s) = 0;
	virtual void visit(compState* s) = 0;
	virtual void visit(never* state) = 0;
	virtual void visit(featStateDecorator* s) = 0;
};

#endif
