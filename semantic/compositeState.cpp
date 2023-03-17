#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <cmath>
#include <cassert>
#include <vector>
#include <iterator>

#include "compositeState.hpp"
#include "compositeTransition.hpp"

#include "programState.hpp"
#include "process.hpp"
#include "never.hpp"

#include "stateVisitor.hpp"

/**
 * Adds the global variables in the memory chunk.
 *
 * Does not set the payloadHash.
 */
compState::compState(const std::string& name) 
	: state(variable::V_COMP_S, name)
	, n(nullptr)
{
}

compState::compState(const compState* other)
	: state(other)
	, n(nullptr)
{
	if(other->n) {
		n = getTVariable<state*>(other->n->getLocalName());
		assert(n && dynamic_cast<never*>(n));
	}
}

compState* compState::deepCopy(void) const {
	return new compState(this);
}

compState::~compState() {

}

void compState::addState(state* s) {
	assert(s);
	_addVariable(s);
}

void compState::addNeverState(state* s) {
	assert(s);
	n = s;
	addState(s);
}

void compState::assign(const variable* sc) {

	variable::assign(sc);

	if(n) {
		n = sc->getTVariable<state*>(n->getLocalName());
		assert(n && dynamic_cast<never*>(n));
	}
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

/*byte compState::compare(const state& s2) const {
	auto compS2 = dynamic_cast<const compState&>(s2);
	for(auto [n1, s1] : subStates) {
		auto subStateS2 = compS2.getState(n1);
		if(subStateS2 == nullptr || subStateS2->hash() != s1->hash())
			return STATES_DIFF;
	}
	return STATES_SAME_S1_FRESH;
}*/

void compState::print(void) const {
	for(auto s : getSubStates()) {
		s->print();
		printf(" ----------------------------------------- \n");
	}
	printf("prob : %lf\n", prob);
	printf(" ****************************************** \n\n");
}

void compState::printTexada(void) const {
	variable::printTexada();
	printf("..\n");
}

/*void compState::printGraphViz(unsigned long i) const {
	auto subStates = getTVariables<state*>();
	for(auto s : subStates)
		s->printGraphViz(i);
}*/

void CartesianRecurse(std::vector<std::vector<transition*>> &accum, std::vector<transition*> stack, std::vector<std::vector<transition*>> sequences, int index) {
    std::vector<transition*> sequence = sequences[index];
    for (auto t : sequence){       
        stack.push_back(t);
        if (index == 0)
            accum.push_back(stack);
        else
            CartesianRecurse(accum, stack, sequences, index - 1);
        stack.pop_back();
    }
}

std::vector<std::vector<transition*>> CartesianProduct(std::vector<std::vector<transition*>> sequences) {
    std::vector<std::vector<transition*>> accum;
    std::vector<transition*> stack;
    if (sequences.size() > 0)
        CartesianRecurse(accum, stack, sequences, sequences.size() - 1);
    return accum;
}

/**
 * Returns a list of all the executable transitions (for all the processes).
 * EFFECTS: None. (It CANNOT have any!)
 * WARNING:
 * 	In the end, does NOT (and must NEVER) modify the state payload.
 */
std::list<transition*> compState::executables(void) const {

	std::list<transition*> execs;
	std::vector<std::vector<transition*>> stateTransList;

	for(auto s : getSubStates()) {
		auto Ts = s->executables();
		
		if(Ts.size() == 0)
			return execs;

		stateTransList.push_back(std::vector<transition*>{ std::begin(Ts), std::end(Ts) });
	}

	auto Tss = CartesianProduct(stateTransList);
	for (auto Ts : Tss) {
		execs.push_back(new compTransition(const_cast<compState*>(this), Ts));
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

#include <iostream>

state* compState::apply(const transition* trans) {
	
	auto compTrans = dynamic_cast<const compTransition*>(trans);
	assert(compTrans);

	for(auto trans : compTrans->Ts) {
		//std::cout << trans->src->getLocalName() << std::endl;
		auto s = getSubState(trans->src->getLocalName());
		assert(s);
		s->apply(trans);
	}

	prob *= trans->prob;
	origin = trans;

	return this;
}

bool compState::nullstate(void) const {
	for(auto elem : getSubStates())
		if(!elem->nullstate())
			return false;
	return true;
}

bool compState::endstate(void) const {
	for(auto elem : getSubStates())
		if(!elem->endstate())
			return false;
	return true;
}

bool compState::isAccepting(void) const {
	for(auto elem : getSubStates())
		if(elem->isAccepting())
			return true;
	return false;
}

bool compState::safetyPropertyViolation(void) const {
	return n? n->safetyPropertyViolation() : false;
}

state* compState::getNeverClaim(void) const {
	return n? n : (parent? dynamic_cast<state*>(parent)->getNeverClaim() : nullptr);
}

state* compState::getSubState(const std::string& name) const {
	return getTVariable<state*>(name);
}

std::list<state*> compState::getSubStates(void) const {
	return getTVariables<state*>();
}

void compState::printHexadecimal(void) const {
	assert(false);
}

std::list<transition*> compState::transitions(void) const {
	std::list<transition*> res;
	for(auto s : getSubStates())
		res.merge(s->transitions());
	return res;
 }

 void compState::accept(stateVisitor* visitor) {
	visitor->visit(this);
}