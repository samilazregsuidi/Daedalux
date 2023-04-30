#include "boolVariable.hpp"

#include "boolSymNode.hpp"

boolVar::boolVar(const boolSymNode* sym, unsigned int index)
	: primitiveVariable(sym, index)
{
}

variable* boolVar::deepCopy(void) const {
	return new boolVar(*this);
}

int boolVar::operator ++ (void) {
	assert(false);
}

int boolVar::operator -- (void) {
	assert(false);
}

int boolVar::operator ++ (int) {
	assert(false);
}

int boolVar::operator -- (int) {
	assert(false);
}

boolVar::operator std::string(void) const {

	char buffer[128];
	if(getValue() == 1)
		sprintf(buffer, "0x%-4lx:   %-23s = true\n", getOffset(), getFullName().c_str());
	else
		sprintf(buffer, "0x%-4lx:   %-23s = false\n", getOffset(), getFullName().c_str());
	return buffer;
}

void boolVar::print(void) const {
	printf("%s", std::string(*this).c_str());
}

void boolVar::printTexada(void) const {
	if(varSym->isPredefined())
		return;

	if(getValue() == 1)
		printf("%s = true\n", getFullName().c_str());
	else
		printf("%s = false\n", getFullName().c_str());
}