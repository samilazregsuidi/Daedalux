#ifndef NEVER_STATE_H
#define NEVER_STATE_H

#include <list>
#include <map>
#include <tuple>
#include <cassert>

#include "thread.hpp"

typedef char byte;

class progState;
class transition;
class RVTransition;

#include "symbols.hpp"

class astNode;
class expr;

class fsm;
class fsmNode;
class fsmEdge;

class state;
class payload;

class variable;
class channel;

class exprArgList;
class exprRArgList;


// A state mask gives for every process the pid, a pointer to its symtab node
// and its offset in the payload
class never : public thread {
public:
	friend class state;

	never(const seqSymNode* sym, const fsmNode* start);

	never(const never* other);

	never* deepCopy(void) const override;

	void init(void) override;

	void print(void) const override;

	std::list<transition*> executables(void) const override;

	state* apply(const transition* trans) override;

	// Expression evaluation (flag)
	#define EVAL_EXECUTABILITY 0
	#define EVAL_EXPRESSION 1

	int eval(const astNode* exp, byte flag) const override;

	int eval(const fsmEdge* edge, byte flag) const override;

	bool isAccepting(void) const override;

	state* getNeverClaim(void) const override;

	//byte compare(const state& s2) const override;
};

#endif