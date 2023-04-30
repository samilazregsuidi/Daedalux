#ifndef MTYPE_VARIABLE_H
#define MTYPE_VARIABLE_H

#include "primitiveVariable.hpp"

class mtypeSymNode;

class mtypeVar : public primitiveVariable {
public:
	mtypeVar(const mtypeSymNode* sym, unsigned int index =  0);

	variable* deepCopy(void) const override;

	~mtypeVar() override {}

	virtual void init(void) override;

	int operator ++ (void) override;

	int operator -- (void) override;

	int operator ++ (int) override;

	int operator -- (int) override;

	operator std::string(void) const override;

	void print(void) const override;

	void printTexada(void) const override;
};

class cmtypeSymNode;

class cmtypeVar : public primitiveVariable {
public:
	cmtypeVar(const cmtypeSymNode* sym);

	~cmtypeVar() override {}

	void setValue(int value) override;
	
	virtual int getValue(void) const override;

	variable* deepCopy(void) const override;

	int operator = (const primitiveVariable& rvalue) override;

	int operator ++ (void) override;

	int operator -- (void) override;

	int operator ++ (int) override;

	int operator -- (int) override;

	operator std::string(void) const override;

	void print(void) const override;

	void printTexada(void) const override;
};

#endif