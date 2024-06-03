/*#ifndef BOOL_VARIABLE_H
#define BOOL_VARIABLE_H

#include "scalarVar.hpp"

class boolSymNode;


class boolVar : public scalar<bool, variable::V_BOOL> {
public:
	boolVar(bool initValue = false);

	boolVar(const std::string& name, bool initValue = false);

	boolVar* deepCopy(void) const override;

	~boolVar() override {}

	float delta(const variable* other) const override;
	
	void printDelta(const variable* other) const override;

	operator std::string(void) const override;

	void print(void) const override;

	void printTexada(void) const override;

	void printCSV(std::ostream &out) const override;

	void printCSVHeader(std::ostream &out) const override;
};

#endif*/