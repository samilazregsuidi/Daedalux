#ifndef STATE_DECORATOR_H
#define STATE_DECORATOR_H

#include "state.hpp"

// State
class stateDecorator : public state {
public:

	stateDecorator(state* wrappee);

	stateDecorator(const stateDecorator& s);

	stateDecorator(const stateDecorator* other);

	~stateDecorator() override;

	/****************************************************/

	/*virtual void setValue(int value) = 0;
	
	virtual int getValue(void) const = 0;

	virtual int operator = (const variable& rvalue) = 0;

	virtual int operator ++ (void) = 0;

	virtual int operator -- (void) = 0;

	virtual int operator ++ (int) = 0;

	virtual int operator -- (int) = 0;

	virtual bool operator == (const variable& other) const = 0;

	virtual bool operator != (const variable& other) const = 0;*/

	/****************************************************/

	std::string getFullName(void) const override;

	std::string getLocalName(void) const override;

	variable::Type getType(void) const override;

	void setGlobal(bool global) override;

	bool isGlobal(void) const override;

	void setPredef(bool predef) override;

	bool isPredef(void) const override;

	void setHidden(bool hidden) override;

	bool isHidden(void) const override;

	void assign(const variable* sc) override;

	void setParent(variable* parent) override;

	variable* getParent(void) const override;

	unsigned int getVariableId(void) const override;

	/**********************************************************/

	operator std::string(void) const override;

	void print(void) const override;

	void printTexada(void) const override;

	void printCSV(std::ostream& out) const override;

	void printCSVHeader(std::ostream& out) const override;

	/************************************************************/

	//void addField(const std::string& name, variable* var) override;

	void _addVariable(variable* subVar) override;

	void _rmVariable(const variable* var) override;

	bool hasVariables(void) const override;

	std::list<variable*> getVariables(void) const override;

	void clearVariables(void) override;

	//std::list<variable*> addVariables(const varSymNode* sym) override;

	//std::list<variable*> createVariables(const varSymNode* sym) override;

	//variable* addVariable(const varSymNode* varSym) override;

	std::unordered_map<std::string, variable*> getVariablesMap(void) const override;

	unsigned long hash(void) const override;

	void init(void) override;

	std::list<transition*> executables(void) const override;

	void apply(transition* trans) override;

	bool nullstate(void) const override;

	bool endstate(void) const override;

	bool isAccepting(void) const override;

	bool safetyPropertyViolation(void) const override;

	state* getNeverClaim(void) const override;

	const transition* getOrigin(void) const override;

	double getProbability(void) const override;

	byte compare(const state& s2) const override;

	float delta(const variable* s2, bool considerInternalVariables) const override;

	std::vector<std::shared_ptr<statePredicate>> getPredicates(void) const override;

	std::list<transition*> transitions(void) const override;

	void accept(stateVisitor* visitor) override;

public:
	state* wrappee;
};

#endif