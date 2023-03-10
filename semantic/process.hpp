#ifndef PROCESS_STATE_H
#define PROCESS_STATE_H

#include <list>
#include <map>
#include <tuple>
#include <cassert>

#include "thread.hpp"

typedef char byte;

#include "astNode.hpp"
#include "automata.hpp"
#include "state.hpp"
#include "programState.hpp"

// A state mask gives for every process the pid, a pointer to its symtab node
// and its offset in the payload
class process : public thread {
public:
	friend class state;

	process(const seqSymNode* sym, const fsmNode* start, byte pid, unsigned int index = 0);

	process(const process& other);

	process(const process* other);

	process* deepCopy(void) const override;

	void init(void) override;

	byte getPid(void) const;

	void setPid(byte pid);

	void print(void) const override;

	std::list<transition*> transitions(void) const override;

	void setProgState(progState* newS);

	progState* getProgState(void) const;

	std::list<transition*> executables(void) const override;

	state* apply(const transition* trans) override;

	// Expression evaluation (flag)
	#define EVAL_EXECUTABILITY 0
	#define EVAL_EXPRESSION 1

	int eval(const astNode* exp, byte flag) const override;

	int eval(const fsmEdge* edge, byte flag) const override;

	bool isAccepting(void) const override;

	state* getNeverClaim(void) const override;

	void accept(stateVisitor* visitor) override;

protected:
	byte pid;
};

#endif