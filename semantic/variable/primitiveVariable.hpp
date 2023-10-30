#ifndef PRIMITIVE_VARIABLE_H
#define PRIMITIVE_VARIABLE_H

#include "variable.hpp"

class primitiveVariable : public variable {
public:

	primitiveVariable(const varSymNode* const sym, unsigned int index =  0);

	primitiveVariable(Type varType, unsigned int index =  0);

	primitiveVariable(const primitiveVariable& other);
	
	primitiveVariable(const primitiveVariable* other);

	variable* deepCopy(void) const override;

	bool isGlobal(void) const override;

	size_t getSizeOf(void) const override;

	/****************************************************/

	virtual void setValue(int value) ;
	
	virtual int getValue(void) const ;

	virtual void init(void) ;

	virtual void reset(void) override;

	virtual int operator = (const primitiveVariable& rvalue) ;

	virtual int operator ++ (void) ;

	virtual int operator -- (void) ;

	virtual int operator ++ (int) ;

	virtual int operator -- (int) ;

	virtual bool operator == (const primitiveVariable& other) const ;

	virtual bool operator != (const primitiveVariable& other) const ;

	/****************************************************/

	operator std::string(void) const override;

	virtual void print(void) const override;

	virtual void printTexada(void) const override;

	virtual void printDaikon(std::ostream &out) const override;

protected:
	const varSymNode* const varSym;
	unsigned int index;
};

class constVar : public primitiveVariable {
public:

	constVar(int value, variable::Type type, int lineNb);

	variable* deepCopy(void) const override;

	int getValue(void) const override;

	void setValue(int value) override;

	int operator = (const primitiveVariable& rvalue) override;

	int operator ++ (void) override;

	int operator -- (void) override;

	int operator ++ (int) override;

	int operator -- (int) override;

private:
	int value;
	int lineNb;

};

class process;
class pidSymNode;

class PIDVar : public primitiveVariable {
public:
	PIDVar(const pidSymNode* sym = nullptr, unsigned int bound =  0);

	variable* deepCopy(void) const override;

	process* getRefProcess(void) const;
	
	void setRefProcess(process* newRef);

	void assign(const variable* sc) override;

private:
	process* ref;
};

#endif