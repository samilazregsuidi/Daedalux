#include "elementStack.hpp"

#include "state.hpp"
#include <iostream>

elementStack::element::element(void) : current_state(nullptr), init(false), depth(0) {}

elementStack::element::element(std::shared_ptr<state> s, unsigned int depth) : current_state(s), init(true), depth(depth)
{
  std::list<std::shared_ptr<state>> sPost_;
  auto successors = s->Post();
  for (auto & p : successors) {
    std::shared_ptr<state> postState(p);
    sPost_.push_back(postState);
  }
  Post = sPost_;
}

// elementStack::element::~element() {
// 	if(s)
// 		delete s;
// 	for(auto p : Post)
// 		delete p;
// }

void elementStack::push(std::shared_ptr<state> s, int depth)
{
  std::shared_ptr<element> elem = std::make_shared<element>(s, depth);
  stackElem.push(elem);
  setElem.insert(s->hash());
}

void elementStack::pop(void)
{
  auto topElem = stackElem.top();
  auto hash = topElem->current_state->hash();
  stackElem.pop();
  setElem.erase(hash);
}

void elementStack::pop(unsigned int numberOfElements)
{
  while (!stackElem.empty() && numberOfElements > 0) {
    --numberOfElements;
    pop();
  }
}

std::shared_ptr<elementStack::element> elementStack::top(void) const
{
  if (stackElem.empty())
    return nullptr;
  return stackElem.top();
}

bool elementStack::isIn(unsigned long elem) const { return setElem.find(elem) != setElem.end(); }

bool elementStack::isIn(const elementStack::element & elem) const { return isIn(elem.current_state->hash()); }

bool elementStack::empty(void) const { return stackElem.empty(); }