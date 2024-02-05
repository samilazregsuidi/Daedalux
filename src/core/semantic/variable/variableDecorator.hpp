#ifndef VARIABLE_DECORATOR_H
#define VARIABLE_DECORATOR_H

#include "variable.hpp"

class variableDecorator : public variable {
public:

	variableDecorator(variable* wrappee);

	variableDecorator(const variableDecorator& other);
	
	variableDecorator(const variableDecorator* other);

	virtual variableDecorator* deepCopy(void) const = 0;

	~variableDecorator() override;

	void init(void) override;

	/****************************************************/

	/*virtual void setValue(int value) = 0;
	
	virtual int getValue(void) const = 0;

	virtual int operator = (const variable& rvalue) = 0;

	virtual int operator ++ (void) = 0;

	virtual int operator -- (void) = 0;

	virtual int operator ++ (int) = 0;

	virtual int operator -- (int) = 0;*/

	bool operator == (const variable* other) const override = 0;

	bool operator != (const variable* other) const override = 0;

	/****************************************************/

	std::string getFullName(void) const override;

	std::string getLocalName(void) const override;

	variable::Type getType(void) const override;

	bool isGlobal(void) const override;

	void assign(const variable* sc) override;

	void setParent(variable* parent) override;

	variable* getParent(void) const override;

	unsigned int getVariableId(void) const override;

	/**********************************************************/

	virtual operator std::string() const override; 

	virtual void print(void) const override;

	virtual void printTexada(void) const override;

	virtual void printCSV(std::ostream &out) const override;

	virtual void printCSVHeader(std::ostream &out) const override;

	virtual void printHexadecimal(void) const override;

	/************************************************************/

	virtual size_t getSizeOf(void) const override;

	size_t getOffset(void) const override;

	size_t getEndOffset(void) const override;

	void setPayload(payload* payLoad) override;
	
	payload* getPayload(void) const override;

	/************************************************************/

	void addRawBytes(size_t size) override;

	//void addField(const std::string& name, variable* var) override;

	void _addVariable(variable* subVar) override;

	void _rmVariable(const variable* var) override;

	bool hasVariables(void) const override;

	std::list<variable*> getVariables(void) const override;

	channel* getChannel(const std::string& name) const override;

	void clearVariables(void) override;

	//std::list<variable*> addVariables(const varSymNode* sym) override;

	//std::list<variable*> createVariables(const varSymNode* sym) override;

	//variable* addVariable(const varSymNode* varSym) override;

	variable* getVariable(const std::string& name) const override;

	std::map<std::string, variable*> getVariablesMap(void) const override;


	unsigned long hash(void) const override;

protected:
	variable* wrappee;
};

#endif