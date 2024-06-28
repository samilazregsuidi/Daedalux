#ifndef UTYPE_VARIABLE_H
#define UTYPE_VARIABLE_H

#include "primitiveVariable.hpp"

class utypeSymNode;

class utypeVar : public primitiveVariable {
public:
	utypeVar(const utypeSymNode* sym, unsigned int index =  0);

	utypeVar(const utypeVar* other);

	~utypeVar() override {}

	variable* deepCopy(void) const override;

	size_t getSizeOf(void) const override;

	int operator = (const primitiveVariable& rvalue) override;

	int operator ++ (void) override;

	int operator -- (void) override;

	int operator ++ (int) override;

	int operator -- (int) override;

	bool operator == (const variable* other) const override;

	bool operator != (const variable* other) const override;

	float delta(const variable* other, bool considerInternalVariables) const override;

	void printDelta(const variable* other, bool considerInternalVariables) const override;

	std::vector<std::shared_ptr<statePredicate>> getPredicates(void) const override;

	std::list<variable *> getDelta(const variable * v2, bool considerInternalVariables) const override;

	operator std::string(void) const override;

	void print(void) const override;

	void printTexada(void) const override;

	void printCSV(std::ostream& out) const override;

	void printCSVHeader(std::ostream& out) const override;
};

#endif