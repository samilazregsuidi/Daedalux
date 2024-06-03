#ifndef NEVER_STATE_H
#define NEVER_STATE_H

#include <list>
#include <map>
#include <tuple>
#include <cassert>

#include "thread.hpp"

// A state mask gives for every process the pid, a pointer to its symtab node
// and its offset in the payload
class never : public thread {
public:
	friend class state;

	never(const std::string& name, const fsmNode* start, ubyte pid);

	never(const never& other);

	never* deepCopy(void) const override;

	void init(void) override;

	//operator std::string(void) const override;

	void print(void) const override;

	void printCSVHeader(std::ostream &out) const override;

	void printCSV(std::ostream &out) const override;

	std::list<transition*> executables(void) const override;

	std::list<transition*> transitions(void) const override;

	void apply(transition* trans) override;

	// Expression evaluation (flag)
	#define EVAL_EXECUTABILITY 0
	#define EVAL_EXPRESSION 1

	int eval(const astNode* exp, byte flag) const override;

	int eval(const fsmEdge* edge, byte flag) const override;

	bool isAccepting(void) const override;

	state* getNeverClaim(void) const override;

	bool safetyPropertyViolation(void) const override;

	void accept(stateVisitor* visitor) override;

	//byte compare(const state& s2) const override;
};

#endif