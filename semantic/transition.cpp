#include "transition.hpp"

#include "deleteTransVisitor.hpp"

#include "state.hpp"

#include <assert.h>
#include <iterator>
#include <algorithm>
#include <iostream>

/*static*/ transition* transition::sample(const std::list<transition*>& transList) {
	if(transList.size() == 0)
		return nullptr;
	auto it = transList.begin();
	std::advance(it, rand() % transList.size());
	return *it;
}

/*static*/ void transition::erase(const std::list<transition*>& list) {
	/*delTransitionVisitor del;
	for(auto t : list)
		t->accept(&del);
	del.deleteVisited();*/
	//assert(false);
	for(auto t : list)
		delete t;
}

transition::transition(state* s) 
	: parent(nullptr)
	, src(s)
	, dst(nullptr)
{
	assert(s);
}

transition::transition(const transition* other) 
	: parent(nullptr)
	, src(other->src)
	, dst(other->dst)
	, prob(other->prob)
	, lines(other->lines)
{
	assert(src);

	for(auto t : other->subTs) {
		add(t->deepCopy());
	}
}

transition::~transition(){
	auto copy = subTs;
	for(auto t : copy)
		delete t;
	
	if(dst)
		dst->origin = nullptr;
	if(parent)
		parent->detach(this);

	assert(subTs.empty());
}

void transition::add(transition* t) {
	if(t == nullptr)
		return;
	t->parent = this;
	subTs.push_back(t);
}

void transition::add(const std::list<transition*>& Ts) {
	for(auto t : Ts)
		add(t);
}

void transition::detach(void) {
	if(parent)
		parent->detach(this);
	auto copy = subTs;
	for(auto t : copy)
		detach(t);
}

void transition::detach(transition* t) {
	auto it = std::find(subTs.begin(), subTs.end(), t);
	assert(it != subTs.end());
	(*it)->parent = nullptr;
	subTs.erase(it);
}

void transition::detach(const std::list<transition*>& Ts) {
	for(auto t : Ts)
		detach(t);
}

double transition::getProbability(void) const {
	return prob;
}