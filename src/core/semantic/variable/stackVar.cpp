#include "stackVar.hpp"

stackVar::stackVar(const std::string& name) 
	: variable(name, V_STACK)
	, front_i(0)
	, back_i(0)
	, length(0)
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
	return varList[front_i];
}

void stackVar::pop_front(void)
{
	assert(!empty());

	varList[front_i++]->reset();

	if(front_i == capacity())
		front_i = 0;

	length--;
}

void stackVar::push_front(const argList& var)
{
	assert(!full());

	*(varList[--front_i]) = var;

	if(front_i == 0)
		front_i = capacity() - 1;

	length++;
}

variable* stackVar::back(void) const
{
	return varList[back_i];
}

void stackVar::pop_back(void)
{
	assert(!empty());

	varList[back_i--]->reset();

	if(back_i == 0)
		back_i = capacity() - 1;

	length--;
}

void stackVar::push_back(const argList& var)
{
	assert(!full());

	*(varList[++back_i]) = var;

	if(back_i == capacity())
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

void stackVar::clear(void)
{
	front_i = 0;
	back_i = 0;
	length = 0;

	for(auto var : varList)
		var->reset();
	length = 0;
}

bool stackVar::operator==(const variable* other) const
{
	auto cast = dynamic_cast<const stackVar*>(other);
	if(!cast)
		return false;

	if(length != cast->length)
		return false;

	if((front_i != cast->front_i) || (back_i != cast->back_i))
		return false;

	auto it = varList.begin();
	auto cast_it = cast->varList.begin();
	
	return variable::==(other);
}

bool stackVar::operator!=(const variable* other) const
{
	return !(*this == other);
}

variable* stackVar::operator=(const variable* other)
{
	variable::operator=(other);
	auto cast = dynamic_cast<const stackVar*>(other);
	if(cast)
	{
		front_i = cast->front_i;
		back_i = cast->back_i;
		length = cast->length;
	}
	else
		assert(false);
	return this;
}