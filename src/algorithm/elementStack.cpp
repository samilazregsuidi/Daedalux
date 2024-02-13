#include "elementStack.hpp"

#include "state.hpp"
#include <iostream>

elementStack::element::element(void)
	: s(nullptr)
	, init(false)
{}
    
elementStack::element::element(std::shared_ptr<state> s, unsigned int depth)
	: s(s)
	, init(true)
	, depth(depth)
{
	std::list<std::shared_ptr<state>> sPost_;
	for (auto & p : s->Post()) {
		std::shared_ptr<state> postState(p);
		sPost_.push_back(postState);
	}
	Post = sPost_;
	/*if(Post.size() == 0){
		Post = s->Post();
		//assert(false);
	}*/
}

// elementStack::element::~element() {
// 	if(s)
// 		delete s;
// 	for(auto p : Post)
// 		delete p;
// }


void elementStack::push(std::shared_ptr<state> s, int depth){
	std::shared_ptr<element> elem(new element(s, depth));
	stackElem.push(elem);
	setElem.insert(s->hash());
}
    
void elementStack::pop(void) {
	auto t = stackElem.top();
	auto hash = t->s->hash();
	
	stackElem.pop();
	setElem.erase(hash);
}

std::shared_ptr<elementStack::element> elementStack::top(void) const {
	if (stackElem.empty())
		return nullptr;
	return stackElem.top();
}

bool elementStack::isIn(unsigned long elem) const {
	return setElem.find(elem) != setElem.end();
}

bool elementStack::isIn(const elementStack::element& elem) const {
	return isIn(elem.s->hash());
}

bool elementStack::empty(void) const {
	return stackElem.empty();
}