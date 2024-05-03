#include "boolVar.hpp"
#include <cstdio>

boolVar::boolVar(bool initValue) 
  : primitive<bool>("", variable::V_BOOL, initValue)
  {}

boolVar::boolVar(const std::string& name, bool initValue) 
  : primitive<bool>(name, variable::V_BOOL, initValue)
  {}

boolVar* boolVar::deepCopy(void) const {
	return new boolVar(*this);
}


float boolVar::delta(const variable* other) const {
	auto otherVar = dynamic_cast<const boolVar*>(other);
	if(!otherVar)
		return 1;
	return (getValue() != otherVar->getValue()) ? 1 : 0;
}

void boolVar::printDelta(const variable* other) const {
	auto otherVar = dynamic_cast<const boolVar*>(other);
	if(!otherVar)
		return;
	if(getValue() != otherVar->getValue())
		printf("%s: %s -> %s\n", getFullName().c_str(), getValue() ? "true" : "false", otherVar->getValue() ? "true" : "false");
}


boolVar::operator std::string(void) const {
	char buffer[128];
	if(getValue() == 1)
		snprintf(buffer, sizeof(buffer), "0x%-4lx:   %-23s = true\n", getOffset(), getFullName().c_str());
	else
		snprintf(buffer, sizeof(buffer), "0x%-4lx:   %-23s = false\n", getOffset(), getFullName().c_str());
	return buffer;
}

void boolVar::print(void) const {
	printf("%s", std::string(*this).c_str());
}

void boolVar::printTexada(void) const {
	if(predef)
		return;

	auto val = getValue() ? "true" : "false";
	printf("%s = %s\n", getFullName().c_str(), val);
}

void boolVar::printCSVHeader(std::ostream &out) const {
	if(predef)
		return;
	out << getFullName() << ",";
}

void boolVar::printCSV(std::ostream &out) const {
	if(predef)
		return;
	auto val = getValue() ? "true" : "false";
	out << val << ",";
}