#include "transition.hpp"

#include "deleteTransVisitor.hpp"

#include "state.hpp"

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <iterator>

/*static*/ transition * transition::sampleUniform(const std::list<transition *> & transList)
{
  if (transList.size() == 0)
    return nullptr;
  auto it = transList.begin();
  std::advance(it, rand() % transList.size());
  return *it;
}

/*static*/ transition * transition::sampleNonUniform(const std::list<transition *> & transList)
{
  if (transList.size() == 0)
    return nullptr;
  double threshold = ((double)rand()) / RAND_MAX;
  double acc = 0.0;
  for (auto t : transList) {
    acc += t->prob;
    if (threshold < acc)
      return t;
  }
  return nullptr;
}

transition * transition::select(const std::list<transition *> & transList, const std::string & action)
{
  for (auto t : transList)
    if (t->action == action)
      return t;
  std::cerr << "Transition not found: " << action << std::endl;
  return nullptr;
}

void transition::erase(const std::list<transition *> & list)
{
  for (auto t : list)
    delete t;
}

transition::transition(state * s) : parent(nullptr), src(s), dst(nullptr), prob(1.)
{
  assert(prob >= 0 && prob <= 1);
  assert(s);
}

transition::transition(const transition * other)
    : parent(nullptr), src(other->src), dst(other->dst), prob(other->prob), action(other->action)
{
  assert(src);
  assert(prob >= 0 && prob <= 1);
}

transition::~transition()
{
  if (dst)
    dst->origin = nullptr;
  // assert(subTs.empty());
}

double transition::getProbability(void) const { return prob; }

//transition * transition::deepCopy(void) const { return new transition(this); }

void transition::accept(transitionVisitor * visitor) { visitor->visit(this); }