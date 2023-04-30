#include "utypeVariable.hpp"

#include "cassert"

#include "symbols.hpp"

utypeVar::utypeVar(const utypeSymNode* sym, unsigned int index)
	: primitiveVariable(sym, index)
{
}

utypeVar::utypeVar(const utypeVar* other) 
	: primitiveVariable(other)
{
	assert(getSizeOf() == other->getSizeOf());
}

int utypeVar::operator = (const primitiveVariable& rvalue) {
	rvalue;
	assert(false);
}

size_t utypeVar::getSizeOf(void) const {
	return variable::getSizeOf();
}

int utypeVar::operator ++ (void) {
	assert(false);
}

int utypeVar::operator -- (void) {
	assert(false);
}

int utypeVar::operator ++ (int) {
	assert(false);
}

int utypeVar::operator -- (int) {
	assert(false);
}

bool utypeVar::operator == (const primitiveVariable& other) const {
	other;
	assert(false);
}

bool utypeVar::operator != (const primitiveVariable& other) const {
	other;
	assert(false);
}

variable* utypeVar::deepCopy(void) const {
	utypeVar* copy = new utypeVar(this);
	//warning shared payload! 
	return copy;
}

utypeVar::operator std::string(void) const {
	return variable::operator std::string();
}

void utypeVar::print(void) const {
	variable::print();
}

void utypeVar::printTexada(void) const {
	variable::printTexada();
}