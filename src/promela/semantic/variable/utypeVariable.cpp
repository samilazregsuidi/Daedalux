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

//need to call variable::operator==(const variable* other)
bool utypeVar::operator == (const variable* other) const {
	return variable::operator==(other);
}

bool utypeVar::operator != (const variable* other) const {
	return variable::operator!=(other);
}

float utypeVar::delta(const variable* v2) const {
	auto casted = dynamic_cast<const utypeVar*>(v2);
	if(!casted)
		return 1;

	float res = 0;
	for(auto var : varList)
		res += var->delta(v2->getVariable(var->getLocalName()));

	return res / varList.size();
}

void utypeVar::printDelta(const variable* v2) const {
	for(auto var : varList)
		var->printDelta(v2->getVariable(var->getLocalName()));
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

void utypeVar::printCSV(std::ostream& out) const {
	variable::printCSV(out);
}

void utypeVar::printCSVHeader(std::ostream& out) const {
	variable::printCSVHeader(out);
}