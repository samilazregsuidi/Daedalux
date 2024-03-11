#ifndef MTYPE_VARIABLE_H
#define MTYPE_VARIABLE_H

#include "primitiveVar.hpp"

class mtypeVar : public primitive<unsigned char> {
public:
	mtypeVar(const std::string& name, unsigned char initValue = 0);

	mtypeVar* deepCopy(void) const override;

	~mtypeVar() override {}

	virtual void init(void) override;

	int operator ++ (void) override;

	int operator -- (void) override;

	int operator ++ (int) override;

	int operator -- (int) override;

	bool operator == (const std::string& cmtype) const;

	bool operator != (const std::string& cmtype) const;

	float delta(const variable* other) const override;

	void printDelta(const variable* other) const override;

	std::list<variable*> getDelta(const variable* other) const override;

	operator std::string(void) const override;

	std::string getValueName(void) const;

	void print(void) const override;

	void printTexada(void) const override;

	void printCSV(std::ostream &out) const override;

	void printCSVHeader(std::ostream &out) const override;

};

class cmtypeSymNode;

class cmtypeVar : public mtypeVar {
public:
	cmtypeVar(const std::string& name, unsigned char initValue = 0);

	~cmtypeVar() override {}

	cmtypeVar* deepCopy(void) const override;

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