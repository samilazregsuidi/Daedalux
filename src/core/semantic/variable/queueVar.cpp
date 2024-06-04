#include "queueVar.hpp"

queueVar::queueVar(const std::string& name, variable::Type type) 
	: variable(type, name)
	, front_i(0)
	, back_i(0)
	, length(0)
{}

queueVar::queueVar(const queueVar& other)
	: variable(other)
	, front_i(other.front_i)
	, back_i(other.back_i)
	, length(other.length)
{}

variable* queueVar::deepCopy(void) const
{
	return new queueVar(*this);;
}

variable* queueVar::front(void) const
{
	return varList[front_i];
}

void queueVar::pop(void)
{
	assert(!empty());

	varList[front_i++]->reset();

	if(front_i == capacity())
		front_i = 0;

	length--;
}

/*void queueVar::push_front(const paramList& params)
{
	assert(!full());

	auto msg = varList[--front_i];
	params.in(msg);

	if(front_i == 0)
		front_i = capacity() - 1;

	length++;
}*/

variable* queueVar::back(void) const
{
	return varList[back_i];
}

/*void queueVar::pop_back(void)
{
	assert(!empty());

	varList[back_i--]->reset();

	if(back_i == 0)
		back_i = capacity() - 1;

	length--;
}*/

void queueVar::push(const paramList& params)
{
	assert(!full());

	auto msg = varList[back_i++];
	params.in(msg);

	if(back_i == capacity())
		back_i = 0;

	length++;
}

bool queueVar::empty(void) const
{
	return length == 0;
}

bool queueVar::full(void) const
{
	return length == capacity();
}

size_t queueVar::len(void) const
{
	return length;
}

size_t queueVar::capacity(void) const
{
	return varList.size();
}

void queueVar::clear(void)
{
	front_i = 0;
	back_i = 0;
	length = 0;

	for(auto var : varList)
		var->reset();
	length = 0;
}

bool queueVar::operator==(const variable* other) const
{
	auto cast = dynamic_cast<const queueVar*>(other);
	if(!cast)
		return false;

	if(length != cast->length)
		return false;

	if((front_i != cast->front_i) || (back_i != cast->back_i))
		return false;
	
	return variable::operator==(other);
}

bool queueVar::operator!=(const variable* other) const
{
	return !(*this == other);
}

variable* queueVar::operator=(const variable* other)
{
	variable::operator=(other);
	auto cast = dynamic_cast<const queueVar*>(other);
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