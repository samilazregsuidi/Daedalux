#include "compositeTransition.hpp"
#include "transition.hpp"
#include "transitionVisitor.hpp"

#include <cassert>
#include <iostream>
#include <algorithm>

compTransition::compTransition(state* s, const std::list<transition*>& Ts)
	: transition(s)
{
	add(Ts);
	for(auto t : Ts) {
		//lines.merge(t->lines);
		prob *= t->prob;
		if(t->action != "") {
			if(action == "")
				action = t->action;
			else
				assert(action == t->action);
		}
	}
	assert(prob >= 0 && prob <= 1);
}

compTransition::compTransition(const compTransition* other) 
	: transition(other)
{
	for (auto t : other->subTs) {
		add(t->deepCopy());
	}
}

compTransition::~compTransition()
{
	auto copy = subTs;
	for (auto t : copy)
    	delete t;
}

transition* compTransition::deepCopy(void) const {
	return new compTransition(this);
}

void compTransition::add(transition * t)
{
  if (t == nullptr)
    return;
  t->parent = this;
  subTs.push_back(t);
}

void compTransition::add(const std::list<transition *> & Ts)
{
  for (auto t : Ts)
    add(t);
}

std::list<transition *> compTransition::getSubTs(void) const
{
  return subTs;
}

void compTransition::detach(void)
{
  if (parent)
    parent->detach(this);
  auto copy = subTs;
  for (auto t : copy)
    detach(t);
}

void compTransition::detach(transition * t)
{
	auto it = std::find(subTs.begin(), subTs.end(), t);
	assert(it != subTs.end());
	(*it)->parent = nullptr;
	subTs.erase(it);
}

void compTransition::detach(const std::list<transition *> & Ts)
{
	if (parent)
    	parent->detach(this);

	for (auto t : Ts)
		detach(t);
}

/*state* compTransition::fire(void)
{
	assert(src != nullptr);
	assert(dst == nullptr);

	auto copy = src->deepCopy();
	assert(copy);

	assert(src->hash() == copy->hash()); // The copy should have the same hash as the original
	assert(copy->getOrigin() == nullptr);

	copy->apply(this);
	assert(copy->getOrigin() == this);
}*/

bool compTransition::operator==(const transition* other) const {
	auto cast = dynamic_cast<const compTransition*>(other);
	if(!cast)
		return false;

	for(auto t : subTs){
		bool found = false;
		for(auto o : cast->subTs){
			if(*t == o){
				found = true;
				break;
			}
		}
		if(!found)
			return false;
	}
	return true;
}

//buggy
float compTransition::similarity(const transition * other) const
{
	auto cast = dynamic_cast<const compTransition*>(other);
		if (!cast)
	return 0;
	float sim = 0;
	for (auto t : subTs) {
		for (auto t_ : cast->subTs) {
			sim += t->similarity(t_);
    }
  }
  return sim / (subTs.size() * cast->subTs.size());
}

void compTransition::accept(transitionVisitor* visitor) {
	visitor->visit(this);
}

void compTransition::print(void) const {
	std::cout << "Composite transition: ";
	for(auto t : subTs)
		t->print();
	std::cout << std::endl;
}