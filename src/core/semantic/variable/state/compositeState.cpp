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

#include "deleteTransVisitor.hpp"

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
		assert(n);
	}
}

compState* compState::deepCopy(void) const {
	return new compState(this);
}

compState::~compState() {
	/*if(origin)
		delete origin;*/
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
		assert(n);
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

void compState::printCSV(std::ostream &out) const {
	for(auto s : getSubStates()) {
		s->printCSV(out);
	}
}

void compState::printCSVHeader(std::ostream &out) const {
	for(auto s : getSubStates()) {
		s->printCSVHeader(out);
	}
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


std::list<transition*> t_copy(const std::vector<transition*>& Ts) {
	std::list<transition*> res;
	for(auto t : Ts)
		res.push_back(t->deepCopy());
	return res;
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
		
		if(Ts.size() == 0) {
			for(auto stateTransListIt : stateTransList)
				for(auto t : stateTransListIt)
					delete t;

			return std::list<transition*>();
		}

		stateTransList.push_back(std::vector<transition*>{ std::begin(Ts), std::end(Ts) });
	}


	auto Tss = CartesianProduct(stateTransList);
	for (auto Ts : Tss) {
		execs.push_back(new compTransition(const_cast<compState*>(this), t_copy(Ts)));
	}

	for(auto stateTransListIt : stateTransList)
		for(auto t : stateTransListIt)
			delete t;
	
	return execs;
}

/**
 * Applies the transition to the state.
 */

void compState::apply(transition* trans) {
	
	assert(trans->dst == nullptr);

	assert(trans->src->hash() == hash());
	
	//this assert is not valid if manual apply is used
	assert(origin == nullptr);

	auto compTrans = dynamic_cast<const compTransition*>(trans);
	assert(compTrans);

	for(auto t : compTrans->getSubTs()) {
		//std::cout << trans->src->getLocalName() << std::endl;
		auto s = getSubState(t->src->getLocalName());
		assert(s);
		s->apply(t);
	}

	prob *= trans->prob;
	origin = trans;
	
	trans->dst = this;
}

/**
 * Returns a new state that is the result of firing the transition.
*/

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