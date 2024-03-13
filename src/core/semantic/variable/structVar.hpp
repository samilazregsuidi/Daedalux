#ifndef STRUCT_VARIABLE_H
#define STRUCT_VARIABLE_H

#include "variable.hpp"

class structVar : public variable {
public:
	structVar(const std::string& name) 
		: variable(V_STRUCT, name)
	{}

	structVar* deepCopy(void) const override {
		return new structVar(*this);
	}
};

#endif