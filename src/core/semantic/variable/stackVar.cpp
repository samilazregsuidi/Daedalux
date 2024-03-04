#include "stackVar.hpp"

stackVar::stackVar(const std::string& name) 
	: variable(name, V_STACK)
{}

stackVar::stackVar(const stackVar& other)
	: variable(other)
	, front_i(other.front_i)
	, back_i(other.back_i)
	, length(other.length)
{}

stackVar::stackVar(const stackVar* other)
	: variable(other)
	, front_i(other->front_i)
	, back_i(other->back_i)
	, length(other->length)
{}

variable* stackVar::deepCopy(void) const
{
	return new stackVar(*this);;
}

variable* stackVar::front(void) const
{
	auto front_it = varList.begin();
	std::advance(front_it, front_i);
	return *front_it;
}

void stackVar::pop_front(void)
{
	assert(!empty());

	auto front_it = varList.begin();
	std::advance(front_it, front_i);
	*(front_it)->reset();
	if(++front_it == varList.end())
		front_i = 0;

	length--;
}

void stackVar::push_front(const variable* var)
{
	assert(!full());

	auto front_it = varList.begin();
	std::advance(front_it, front_i);
	*front_it = var;
	if(--front_it == varList.begin())
		front_i = capacity() - 1;

	length++;
}

variable* stackVar::back(void) const
{
	auto back_it = varList.begin();
	std::advance(back_it, back_i);
	return *back_it;
}

void stackVar::pop_back(void)
{
	assert(!empty());

	auto back_it = varList.begin();
	std::advance(back_it, back_i);
	*(back_it)->reset();
	if(--back_it == varList.begin())
		back_i = capacity() - 1;

	length--;
}

void stackVar::push_back(const variable* var)
{
	assert(!full());

	auto back_it = varList.begin();
	std::advance(back_it, back_i);
	*back_it = var;
	if(++back_it == varList.end())
		back_i = 0;

	length++;
}

bool stackVar::empty(void) const
{
	return length == 0;
}

bool stackVar::full(void) const
{
	return length == capacity();
}

size_t stackVar::len(void) const
{
	return length;
}

size_t stackVar::capacity(void) const
{
	return varList.size();
}