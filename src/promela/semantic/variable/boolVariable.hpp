#ifndef BOOL_VARIABLE_H
#define BOOL_VARIABLE_H

#include "primitiveVariable.hpp"

class boolSymNode;

class boolVar : public primitive<bool> {
public:
	boolVar(const std::string& name);

	variable* deepCopy(void) const override;

	~boolVar() override {}

	bool operator ++ (void) override;

	bool operator -- (void) override;

	bool operator ++ (int) override;

	bool operator -- (int) override;

	bool operator == (bool value) const;

	bool operator != (bool value) const;

	float delta(const variable* other) const override;
	
	void printDelta(const variable* other) const override;

	operator std::string(void) const override;

	void print(void) const override;

	void printTexada(void) const override;

	void printCSV(std::ostream &out) const override;

	void printCSVHeader(std::ostream &out) const override;
};

#endif