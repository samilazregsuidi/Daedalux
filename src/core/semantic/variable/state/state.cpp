#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "state.hpp"
#include "threadTransition.hpp"

#include "stateVisitor.hpp"
#include <iostream>

/**
 * Adds the global variables in the memory chunk.
 *
 * Does not set the payloadHash.
 */

state::state(variable::Type type, const std::string & name) : variable(type, name), prob(1.0), origin(nullptr), errorMask(0) {}

state::state(const state& other)
	: variable(other)
	, prob(other.prob)
	, origin(nullptr)
	, errorMask(other.errorMask)
{}

state::~state() {
	if(origin)
		delete origin;
}

void state::addError(unsigned int e) { errorMask |= e; }

unsigned int state::getErrorMask(void) const { return errorMask; }

bool state::hasDeadlock(void) const { return executables().size() == 0; }

std::list<state *> state::Post(void) const
{
  std::list<state *> res;
  auto executableTransitions = executables();
  for (auto t : executableTransitions) {
    auto successor = fire(t);
    res.push_back(successor);
  }
  if (res.empty()) {
    // Check if the never claim has more transitions to fire
    auto neverClaim = getNeverClaim();
    std::list<transition *> neverTs;
    if (neverClaim)
      neverTs = neverClaim->executables();
    if (neverClaim && neverTs.empty()) {
      // Deadlock detected because the never claim has no more transitions to fire
      this->errorMask |= ERR_PROPERTY_VIOLATION;
    }
    else {
      // Deadlock detected because there are no more transitions to fire
      this->errorMask |= ERR_DEADLOCK;
    }
    transition::erase(neverTs);
  }
  return res;
}

std::list<state *> state::SafePost(void) const
{
  try {
    return Post();
  }
  catch (std::exception & e) {
    std::cout << "Exception caught in state::SafePost: " << e.what() << std::endl;
    return std::list<state *>();
  }
}

state * state::fire(transition * trans) const
{
  assert(trans->src == this);
  assert(trans->dst == nullptr);

  auto copy = deepCopy();
  assert(copy);
  assert(this != copy);

  assert(this->isSame(copy));       // The copy should have the same hash as the original
  assert(copy->getOrigin() == nullptr); // the origin has been reset to nullptr

  copy->apply(trans);

  // Ensure that the transition has been applied
  assert(copy->origin == trans);
  assert(trans->dst == copy);

  return copy;
}

state* state::fire(const std::list<std::pair<std::string, unsigned int>>& transitions) const {
  auto copy = deepCopy();
  assert(copy);
  assert(this != copy);

  auto execs = this->executables();

  for (auto executable : execs) {
    bool found = true;
    
    for (const auto& [varName, edgeNum] : transitions) {
      auto transition = dynamic_cast<threadTransition*>(executable->getTransition(varName));
      if(!transition) {
        std::cout << "The state " << std::string(*this) << " does not have a transition with the name " << varName << std::endl;
        assert(transition);
      }

      if (transition->getLine() != edgeNum) {
        found = false;
        break;
      }
    }

    if (found) {
      copy->apply(executable);
      return copy;
    }
  }

  delete copy;

  assert(false);
  // If no transition was found, return nullptr
  return nullptr;
}

state* state::fire(const std::pair<std::string, const std::list<unsigned int>>& trans) const {
  auto copy = deepCopy();
  assert(copy);
  assert(this != copy);

  const auto& [varName, edgeNums] = trans;
  for(auto edgeNum : edgeNums)
    copy = copy->fire({{varName, edgeNum}});
  return copy;
}

void state::applyRepeated(const std::list<transition *> & transList)
{
  for (auto t : transList)
    this->apply(t);
}

const transition * state::getOrigin(void) const { return origin; }

double state::getProbability(void) const { return prob; }

byte state::compare(const state & s2) const { return this->isSame(&s2); }

byte state::compare(unsigned long s2Hash) const { return hash() == s2Hash; }

void state::accept(stateVisitor * visitor) { visitor->visit(this); }