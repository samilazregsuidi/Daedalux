#ifndef STATE_H
#define STATE_H
/*
 * ENCODING OF STATES
 * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * A state consists of a set of active processes and of the values of the
 * variables of these processes.  It is thus represented by two elements:
 *
 *  - A list of active processes, and for each a pointer to its symbol table.
 *
 *  - A block of memory containing the variables of these processes.  This is
 *    more efficient than a linked list of values and basically corresponds to
 *    a run-time struct.
 *
 *    The block is divided into sub-blocks, one for each process.  The
 *    list of processes contains the offsets of the sub-block of each process.
 *    One sub-block holds all variable values in the order in which they
 *    were declared in the symbol table of the process.  For each variable, a
 *    predefined chunk of memory is allocated (all variable types are finite
 *    and cannot grow).
 *
 *
 * To test equivalence of two states, it is sufficient to test equivalence of
 * payloadSize, and if equivalent, the whole blocks.
 */

#include <list>
#include <map>
#include <stack>
#include <string>
#include <functional>
#include <memory>

#include "variable.hpp"

typedef char byte;
typedef unsigned char ubyte;

class transition;

class stateVisitor;

// State comparison. Returns:
#define STATES_DIFF 0				// if s1 and s2 are totally different states, meaning s1 is fresh.
#define STATES_SAME_S1_VISITED 1	// if s1 and s2 are identical but s2 is reachable by more products; hence, s1 adds nothing new
#define STATES_SAME_S1_FRESH 2		// if s1 and s2 are identical but s1 has products that were not explored with s2; hence, s1 is fresh
#define STATES_S1_NEVER_VISITED 3


// State
class state : public variable {
public:

	static const unsigned int ERR_DEADLOCK = 0x1;
	static const unsigned int ERR_ASSERT_FAIL = 0x2;

	state(variable::Type type, const std::string& name = "");

	state(const state& s);

	state(const state* other);

	virtual state* deepCopy(void) const = 0 ;

	/**
	 * Frees the memory used by a given state. It does NOT free any symbol tables, FSM or mtypes list.
	 * However, it DOES free:
	 *  - the memory chunk,
	 *  - all state masks of active processes,
	 *  - the state mask of the never claim (if any),
	 *  - all channel references,
	 *
	 * If the keepPayloadAndFeatures parameter is false, it also frees:
	 *  - boolean formula and
	 *  - the state structure itself.
	 *
	 * This parameter is only true when destroying a stack element where the payload and boolean function
	 * are still used in the visited states hashtable.
	 */

	virtual ~state();

	virtual std::list<transition*> executables(void) const = 0;

	virtual std::list<transition*> transitions(void) const = 0;

	virtual bool hasDeadlock(void) const;

	//virtual unsigned int getErrors(void) const = 0;

	void addError(unsigned int e);
	
	unsigned int getErrorMask(void) const;

	// Applying statements

	virtual void apply(transition* trans) = 0;

	virtual void applyRepeated(const std::list<transition*>& transList);
	
	//static state* apply(const state* s, transition* t);

	//static state* applyRepeated(const state* s, const std::list<transition*> transList);

	virtual std::list<state*> Post(void) const;

	//static std::list<std::shared_ptr<state>> Post(const state* s);

	//virtual state* Post(transition* trans) const;

	virtual state* fire(transition* trans) const;

	//static std::shared_ptr<state> Post(transition* trans, const state* s);

	//virtual state* getSubState(const std::string& name) const;

	//size_t _getSizeOf(void) const;

	//size_t getSizeOf(void) const;

	virtual bool safetyPropertyViolation(void) const = 0;

	virtual bool nullstate(void) const = 0;

	virtual bool endstate(void) const = 0;

	virtual bool isAccepting(void) const = 0;

	virtual state* getNeverClaim(void) const = 0;

	virtual const transition* getOrigin(void) const;

	virtual double getProbability(void) const;
	
	/*
	* If the pid of the last process is 'pid' then:
	*  - the stateMask of the process is removed
	*  - the number of processes in the states updated
	*  - the chunk of memory of the process is removed from the state's payload.
	* Does not change the payloadHash.
	*/
	//void killProctype(int pid);

	//void clean(void); // Applies stateKillProctype while this latter function succeeds.

	// State printing

	virtual byte compare(const state& s2) const;

	virtual byte compare(unsigned long s2Hash) const;

	virtual void accept(stateVisitor* visitor);

	// virtual void printCSV(std::ostream& out) const;

public:
	double prob;
	const transition* origin;
	mutable unsigned int errorMask;
	std::string secret;
};

template<> struct std::hash<state*> {
	size_t operator()(const state* s) const noexcept {
		return s->hash();
	}
};

#endif