#include "featuredStateDecorator.hpp"
#include "featuredTransition.hpp"

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
	, choices(tvl->getMgr()->addOne())
	, tvl(tvl)
{
}

featStateDecorator::featStateDecorator(const featStateDecorator* other)
	: stateDecorator(other)
	, features(other->getFeatures())
	, diagram(other->diagram)
	, choices(other->choices)
	, tvl(other->tvl)
	, R(other->R)
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

unsigned long featStateDecorator::hash(void) const {
	return wrappee->hash();
}

ADD featStateDecorator::getFeatures(void) const {
	return features;
}

ADD featStateDecorator::getDiagram(void) const {
	return diagram;
}

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
		auto featTrans = dynamic_cast<featTransition*>(candidate);
		//if(featTrans) {
			//printf("***********EXE STATE FEATURES***********\n");
			//TVL::printBool(features);
			//printf("***********EXE TRANS FEATURES***********\n");
			//TVL::printBool(featTrans->getFeatExpr());
			//printf("**********************\n")
			//TVL::printBool(diagram);
		//}
		if(!featTrans || !(diagram & features & featTrans->getFeatExpr()).IsZero()) {
			execs.push_back(candidate);
		} else {
			delete candidate;
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
void featStateDecorator::apply(transition* trans) {
	
	wrappee->apply(trans);

	assert(features);

	auto ftrans = dynamic_cast<featTransition*>(trans);
	if(ftrans) {
		/*printf("***********APPLY STATE FEATURES***********\n");
		TVL::printBool(features);
		printf("***********APPLY TRANS FEATURES***********\n");
		TVL::printBool(ftrans->getFeatExpr());
		//printf("**********************\n")
		//TVL::printBool(diagram);*/
		features &= ftrans->getFeatExpr();
		assert(!(features * diagram).IsZero());
		choices &= ftrans->getFeatExpr();
		//printf("***********APPLY RES***********\n");
		//TVL::printBool(features);
	}

	wrappee->origin = trans;
	origin = trans;
	trans->dst = this;
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
	return compare(s2.hash(), featS2);
}

#include <iostream>

byte featStateDecorator::compare(unsigned long s2Hash, const ADD& featS2) const {
	byte res = wrappee->compare(s2Hash);
	
	if(res == STATES_DIFF)
		return res;

	/*std::cout << getLocalName() << " feat " << std::endl;
	TVL::printBool(features);
	std::cout << getLocalName() << " R " << std::endl;
	TVL::printBool(featS2);
	std::cout << (features & ~featS2).IsZero() << std::endl;*/

	//if f(s) \ R(s) is empty
	if((features & ~featS2).IsZero())
		return STATES_SAME_S1_VISITED;

	return STATES_SAME_S1_FRESH;
}

void featStateDecorator::accept(stateVisitor* visitor) {
	visitor->visit(this);
}