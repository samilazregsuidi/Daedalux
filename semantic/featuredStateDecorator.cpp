#include "featuredStateDecorator.hpp"
#include "featuredProgramTransition.hpp"

//bad coupling!
#include "tvl.hpp"

#include "stateVisitor.hpp"

#include "ADDutils.hpp"

/**
 * Adds the global variables in the memory chunk.
 *
 * Does not set the payloadHash.
 */

featStateDecorator::featStateDecorator(state* wrappee, const ADD& diagram, const TVL* tvl) 
	: stateDecorator(wrappee)
	, features(tvl->getMgr()->addOne())
	, diagram(diagram)
	, tvl(tvl)
{
}

featStateDecorator::featStateDecorator(const featStateDecorator* other)
	: stateDecorator(other)
	, features(other->getFeatures())
	, diagram(other->diagram)
	, tvl(other->tvl)
{}


state* featStateDecorator::deepCopy(void) const {
	featStateDecorator* copy = new featStateDecorator(this);
	//auto newScope = deepCopy();
	//newScope->setPayload(getPayload()->copy());
	//copy->assign(newScope);
	return copy;
}

/**
 * Frees the memory used by a given state. It does NOT free any symbol tables, FSM or mtypes list.
 * However, it DOES free:
 *  - the memory chunk,
 *  - all state procs of active processes,
 *  - the state proc of the never claim (if any),
 *  - all channel references,
 *
 * If the keepPayloadAndFeatures parameter is false, it also frees:
 *  - boolean formula and
 *  - the state structure itself.
 *
 * This parameter is only true when destroying a stack element where the payload and boolean function
 * are still used in the visited states hashtable.
 */

featStateDecorator::~featStateDecorator() {
}

const ADD& featStateDecorator::getFeatures(void) const {
	return features;
}

const ADD& featStateDecorator::getDiagram(void) const {
	return diagram;
}

/*
 * STATE COMPARISON
 * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * Compares s1 a newly reached state
 *     with s2 a state known to be reachable
 * to see whether s1 is a state that was already visited.
 *
 * When s1 was not yet visited, then we say it's "fresh".
 *
 * Returns:
 * 	- STATES_DIFF 			 if s1 and s2 are totally different states, meaning s1 is fresh.
 * 	- STATES_SAME_S1_VISITED if s1 and s2 are identical but s2 is reachable by more products; hence, s1 adds nothing new
 *  - STATES_SAME_S1_FRESH	 if s1 and s2 are identical but s1 has products that were not explored with s2; hence, s1 is fresh
 */
/*

byte progState::compare(const state& s2) const {

	if(!(*payLoad == *s2.payLoad))	
		return STATES_DIFF;

	// Now that we know both states are identical, we check whether:
	//  s1 -> s2
	//
	// If this holds, then s1 is reachable in less products, which means
	//                that it can be considered as visited.
	// It not,        then s1 is reachable by at least one product that
	//                was not previously explored, so it contains some
	//                fresh info, and exploration must continue.

	// Convention: nullptr means 'true'.
	/*if(!s2Features) return STATES_SAME_S1_VISITED;
	if(!s1->features) return STATES_SAME_S1_FRESH;	// Here we do not check the case in which s2->features != nullptr but still a tautology;
													// There is a compilation parameter CHECK_TAUTOLOGY that can be set to check for
													// tautologies before they end up here.

	if(implies(s1->features, s2Features)) return STATES_SAME_S1_VISITED;
	
	return STATES_SAME_S1_FRESH;
}*/

void featStateDecorator::print(void) const {
	
	wrappee->print();

	printf("\n\n");

	tvl->printBool(features);
}

/**
 * Returns a list of all the executable transitions (for all the processes).
 * EFFECTS: None. (It CANNOT have any!)
 * WARNING:
 * 	In the end, does NOT (and must NEVER) modify the state payload.
 */
std::list<transition*> featStateDecorator::executables(void) const {

	std::list<transition*> candidates = wrappee->executables();
	std::list<transition*> execs;

	for(auto candidate : candidates) {
		auto featTrans = dynamic_cast<featProgTransition*>(candidate);
		if(!featTrans) {
			execs.push_back(candidate);

		} else if(!(features * featTrans->getFeatExpr() * diagram).IsZero()) {
			execs.push_back(candidate);
		}
	}

	return execs;
}

/**
 * Executes a statement and returns the new reached state. The transition must be executable.
 * The preserve parameter controls whether or not the state that is passed is preserved.
 *
 * The features expression of the processTransition is not modified. The value of this expression is
 * copied into the new state. Thus, when this state is destroyed, the features expression of the
 * processTransition is not deleted.
 *
 * assertViolation is a return value set to true in case the statement on the transition was an assert
 * that evaluated to false.
 */
state* featStateDecorator::apply(const transition* trans) {
	
	wrappee->apply(trans);

	auto ftrans = dynamic_cast<const featProgTransition*>(trans);
	if(ftrans) {
		if(!features)
			features = ftrans->getFeatExpr() * diagram;
		else 
			features = features * ftrans->getFeatExpr() * diagram;
	}

	return this;
}

bool featStateDecorator::constraint(const ADD& cst) {
	features &= cst;
	return !features.IsZero();
}

byte featStateDecorator::compare(const state& s2) const {
	byte res = wrappee->compare(s2);
	
	auto featStateS2 = dynamic_cast<const featStateDecorator*>(&s2);
	if(res == STATES_DIFF || !featStateS2) 
		return res;
	
	if(implies(features, featStateS2->getFeatures()))
		return STATES_SAME_S1_VISITED;

	return STATES_SAME_S1_FRESH;
}

byte featStateDecorator::compare(const state& s2, const ADD& featS2) const {
	byte res = wrappee->compare(s2);
	
	auto featStateS2 = dynamic_cast<const featStateDecorator*>(&s2);
	if(res == STATES_DIFF || !featStateS2) 
		return res;
	
	if(implies(features, featS2))
		return STATES_SAME_S1_VISITED;

	return STATES_SAME_S1_FRESH;
}

void featStateDecorator::accept(stateVisitor* visitor) {
	visitor->visit(this);
}