#include <cassert>
#include <cmath>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>

#include "composite.hpp"
#include "compositeTransition.hpp"

#include "deleteTransVisitor.hpp"

#include "stateVisitor.hpp"

/**
 * Adds the global variables in the memory chunk.
 *
 * Does not set the payloadHash.
 */
composite::composite(const std::string & name) : state(variable::V_COMP_S, name), n(nullptr) {}

composite::composite(const composite& other)
	: state(other)
	, n(nullptr)
{
	if(other.n) {
		n = get<state*>(other.n->getLocalName());
		assert(n);
	}
}

composite* composite::deepCopy(void) const {
	return new composite(*this);
}

void composite::addState(state* s) {
	assert(s);
	_addVariable(s);
}

void composite::addNeverState(state * s)
{
  assert(s);
  n = s;
  addState(s);
}

void composite::assign(const variable * sc)
{

  variable::assign(sc);

	if(n) {
		n = sc->get<state*>(n->getLocalName());
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
 *  - STATES_SAME_S1_FRESH	 if s1 and s2 are identical but s1 has products that were not explored with s2; hence, s1 is
 * fresh
 */

/*byte composite::compare(const state& s2) const {
        auto compS2 = dynamic_cast<const composite&>(s2);
        for(auto [n1, s1] : subStates) {
                auto subStateS2 = compS2.getState(n1);
                if(subStateS2 == nullptr || subStateS2->hash() != s1->hash())
                        return STATES_DIFF;
        }
        return STATES_SAME_S1_FRESH;
}*/

void composite::print(void) const
{
  for (auto s : getSubStates()) {
    s->print();
    printf(" ----------------------------------------- \n");
  }
  printf("prob : %lf\n", prob);
  printf(" ****************************************** \n\n");
}

void composite::printTexada(void) const
{
  variable::printTexada();
  printf("..\n");
}

void composite::printCSV(std::ostream & out) const
{
  for (auto s : getSubStates()) {
    s->printCSV(out);
  }
}

void composite::printCSVHeader(std::ostream & out) const
{
  for (auto s : getSubStates()) {
    s->printCSVHeader(out);
  }
}

/*void composite::printGraphViz(unsigned long i) const {
        auto subStates = getTVariables<state*>();
        for(auto s : subStates)
                s->printGraphViz(i);
}*/

void CartesianRecurse(std::vector<std::vector<transition *>> & accum, std::vector<transition *> stack,
                      std::vector<std::vector<transition *>> sequences, int index)
{
  std::vector<transition *> sequence = sequences[index];
  for (auto t : sequence) {
    stack.push_back(t);
    if (index == 0)
      accum.push_back(stack);
    else
      CartesianRecurse(accum, stack, sequences, index - 1);
    stack.pop_back();
  }
}

std::vector<std::vector<transition *>> CartesianProduct(std::vector<std::vector<transition *>> sequences)
{
  std::vector<std::vector<transition *>> accum;
  std::vector<transition *> stack;
  if (sequences.size() > 0)
    CartesianRecurse(accum, stack, sequences, sequences.size() - 1);
  return accum;
}

std::list<transition *> t_copy(const std::vector<transition *> & Ts)
{
  std::list<transition *> res;
  for (auto t : Ts)
    res.push_back(t->deepCopy());
  return res;
}

/**
 * Returns a list of all the executable transitions (for all the processes).
 * EFFECTS: None. (It CANNOT have any!)
 * WARNING:
 * 	In the end, does NOT (and must NEVER) modify the state payload.
 */
std::list<transition *> composite::executables(void) const
{

  std::list<transition *> execs;
  std::vector<std::vector<transition *>> stateTransList;

  for (auto s : getSubStates()) {
    auto Ts = s->executables();

    if (Ts.size() == 0) {
      for (auto stateTransListIt : stateTransList)
        for (auto t : stateTransListIt)
          delete t;

      return std::list<transition *>();
    }

    stateTransList.push_back(std::vector<transition *>{std::begin(Ts), std::end(Ts)});
  }

  auto Tss = CartesianProduct(stateTransList);
  for (auto Ts : Tss) {
    execs.push_back(new compTransition(const_cast<composite *>(this), t_copy(Ts)));
  }

  for (auto stateTransListIt : stateTransList)
    for (auto t : stateTransListIt)
      delete t;

  return execs;
}

/**
 * Applies the transition to the state.
 */

void composite::apply(transition* trans) {
	// This is temporally commented out to make some tests pass
	assert(trans->src != nullptr);
	assert(trans->src != this);
	assert(trans->dst == nullptr); // Apply most not have been called on this transition before
	assert(origin == nullptr);
	auto compTrans = dynamic_cast<const compTransition*>(trans);
	// Ensure that the cast was successful
	assert(compTrans);

	for(auto t : compTrans->getSubTs()) {
		//std::cout << trans->src->getLocalName() << std::endl;
		auto localName = t->src->getLocalName();
		auto s = getSubState(localName);
		assert(s);
		s->apply(t);
	}
	//Todo here - src and dst should not be the same

	prob *= trans->prob;
	origin = trans;
	trans->dst = this;
	assert(trans->src != trans->dst);
}

/**
 * Returns a new state that is the result of firing the transition.
 */

bool composite::nullstate(void) const
{
  for (auto elem : getSubStates())
    if (!elem->nullstate())
      return false;
  return true;
}

bool composite::endstate(void) const
{
  for (auto elem : getSubStates())
    if (!elem->endstate())
      return false;
  return true;
}

bool composite::isAccepting(void) const
{
  for (auto elem : getSubStates())
    if (elem->isAccepting())
      return true;
  return false;
}

bool composite::safetyPropertyViolation(void) const { return n ? n->safetyPropertyViolation() : false; }

state * composite::getNeverClaim(void) const
{
  return n ? n : (parent ? dynamic_cast<state *>(parent)->getNeverClaim() : nullptr);
}

state * composite::getSubState(const std::string & name) const { return get<state *>(name); }

std::list<state *> composite::getSubStates(void) const { return getAll<state *>(); }

std::list<transition *> composite::transitions(void) const
{
  std::list<transition *> res;
  for (auto s : getSubStates())
    res.merge(s->transitions());
  return res;
}

void composite::accept(stateVisitor * visitor) { visitor->visit(this); }
