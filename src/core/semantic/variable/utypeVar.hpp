#ifndef UTYPE_VARIABLE_H
#define UTYPE_VARIABLE_H

#include "primitiveVar.hpp"

class utypeSymNode;

class utypeVar : public variable {
public:
	utypeVar(const std::string& name) 
		: variable(name, V_UTYPE)
	{}
};

#endif