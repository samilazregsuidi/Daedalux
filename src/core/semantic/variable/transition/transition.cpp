#include "transition.hpp"

#include "deleteTransVisitor.hpp"

#include "state.hpp"

#include <assert.h>
#include <iterator>
#include <algorithm>
#include <iostream>

/*static*/ transition* transition::sampleUniform(const std::list<transition*>& transList) {
	if(transList.size() == 0)
		return nullptr;
	auto it = transList.begin();
	std::advance(it, rand() % transList.size());
	return *it;
}

/*static*/ transition* transition::sampleNonUniform(const std::list<transition*>& transList) {	
	if(transList.size() == 0)
		return nullptr;
	double threshold = ((double)rand()) / RAND_MAX;
	double acc = 0.0;
	for(auto t : transList){
		acc += t->prob;
		if(threshold < acc)
			return t;
	}
	return nullptr;
}

/*static*/ transition* transition::select(const std::list<transition*>& transList, const std::string& action) {
	for(auto t : transList)
		if(t->action == action)
			return t;
	return nullptr;
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
	, prob(1.)
{
	assert(s);
}

transition::transition(const transition* other) 
	: parent(nullptr)
	, src(other->src)
	, dst(other->dst)
	, prob(other->prob)
	, lines(other->lines)
	, action(other->action)
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

	//assert(subTs.empty());
}

void transition::add(transition* t) {
	if(t == nullptr)
		return;
	assert(t->parent == nullptr);
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

transition* transition::getTransition(const std::string& stateName) const {

	for(auto t : subTs) {
		if(t->src->getLocalName() == stateName)
			return t;
		else {
			auto res = t->getTransition(stateName);
			if(res)
				return res;
		}
	}
	return nullptr;
}

double transition::getProbability(void) const {
	return prob;
}

bool transition::operator==(const transition* other) const {
	for(auto t : subTs) {
		bool found = false;
		for(auto t_ : other->subTs) {
			if(*t == t_) {
				found = true;
				break;
			}
		}
		if(!found)
			return false;
	}
	return true;
}

float transition::similarity(const transition* other) const {
	float sim = 0;
	for(auto t : subTs) {
		for(auto t_ : other->subTs) {
			sim += t->similarity(t_);
		}
	}
	return sim / (subTs.size() * other->subTs.size());
}