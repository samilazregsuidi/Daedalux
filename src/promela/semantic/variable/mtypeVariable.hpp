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

	bool operator == (const std::string& cmtype) const;

	bool operator != (const std::string& cmtype) const;

	float delta(const variable* other, bool considerInternalVariables) const override;

	void printDelta(const variable* other, bool considerInternalVariables) const override;

	std::list<variable*> getDelta(const variable* other, bool considerInternalVariables) const override;

	operator std::string(void) const override;

	std::string getValueName(void) const;

	void print(void) const override;

	void printTexada(void) const override;

	void printCSV(std::ostream &out) const override;

	void printCSVHeader(std::ostream &out) const override;

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

	void printCSV(std::ostream &out) const override;

	void printCSVHeader(std::ostream &out) const override;
};

#endif