#include "elementStack.hpp"

#include "state.hpp"

elementStack::element::element(void)
	: s(nullptr)
	, init(false)
{}
    
elementStack::element::element(state* s, unsigned int depth)
	: s(s)
	, init(true)
	, depth(depth)
{

	Post = s->Post();
}

elementStack::element::~element() {
	if(s)
		delete s;
	for(auto p : Post)
		delete p;
}

void elementStack::push(state* s, int depth) {
	stackElem.push(new element(s, depth));
	setElem.insert(s->hash());
}
    
void elementStack::pop(void) {
	auto t = stackElem.top();
	auto hash = t->s->hash();
	
	stackElem.pop();
	setElem.erase(hash);

	delete t;
}

elementStack::element* elementStack::top(void) const {
	return stackElem.top();
}

bool elementStack::isIn(unsigned long elem) const {
	return setElem.find(elem) != setElem.end();
}

bool elementStack::isIn(const element& elem) const {
	return isIn(elem.s->hash());
}

bool elementStack::empty(void) const {
	return stackElem.empty();
}