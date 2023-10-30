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

	bool operator == (const primitiveVariable& other) const override;

	bool operator != (const primitiveVariable& other) const override;

	operator std::string(void) const override;

	void print(void) const override;

	void printTexada(void) const override;

	void printCSV(std::ostream& out) const override;
};

#endif