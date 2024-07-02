#ifndef STRUCT_VARIABLE_H
#define STRUCT_VARIABLE_H

#include "variable.hpp"

class structVar : public variable {
public:
	structVar(const std::string& name);

	structVar* deepCopy(void) const override;

	float delta(const variable * v2, bool considerInternalVariables) const override;

	std::vector<std::shared_ptr<statePredicate>> getPredicates(void) const override;

	void printDelta(const variable * v2, bool considerInternalVariables) const override;

	std::list<variable *> getDelta(const variable * v2, bool considerInternalVariables) const override;
};

#endif