#include "utypeVariable.hpp"

#include "cassert"

#include "symbols.hpp"

utypeVar::utypeVar(const utypeSymNode* sym, unsigned int index)
	: primitiveVariable(sym, index)
{
	for(auto field : sym->getUType()->getFields()) {
		addVariables(field);
	}
}

utypeVar::utypeVar(const utypeVar* other) 
	: primitiveVariable(other)
{
}

int utypeVar::operator = (const primitiveVariable& rvalue) {
	rvalue;
	assert(false);
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
	utypeVar* copy = new utypeVar(*this);
	//warning shared payload! 
	return copy;
}

void utypeVar::print(void) const {
	variable::print();
}

void utypeVar::printTexada(void) const {
	variable::printTexada();
}