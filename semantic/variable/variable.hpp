#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include <list>
#include <map>

typedef char byte;
typedef unsigned char ubyte;

class payload;

#include "symbols.hpp"

class varSymNode;

class expr;
class exprArgList;
class exprRArgList;

class channel;

class variable {
public:

	// Primitive types:
	enum Type {
		V_NA,
		V_BIT,
		V_BOOL,
		V_BYTE,
		V_PID,
		V_SHORT,
		V_INT,
		V_UNSGN, 	// not supported yet
		V_MTYPE,
		V_CLOCK ,	// dense time clock
		V_MTYPE_DEF,
		V_CMTYPE,

		//V_FEAT,
		//V_UFEAT,

		// "Special" types:
		V_CHAN,		// Channel: capacity used; children denote message fields
		V_CID,		// Channel reference; capacity and children are not used.
		V_TDEF,		// Type definition: children denote fields of type
		V_INIT,
		V_PROC,		// ProcType: fsm field used; bound denotes the number of initially active processes
		V_INLINE,
		V_UTYPE,	// Type of variable is a user type (basically, a T_TDEF record is being used as the type): utype points to the type record
		V_NEVER,	// Never claim

		V_VARIANT,

		V_STATE,
		V_COMP_STATE
	};

	variable(Type type, const std::string& name = std::string());

	variable(const variable& other);
	
	variable(const variable* other);

	virtual variable* deepCopy(void) const = 0;

	virtual ~variable();

	/****************************************************/

	virtual void init(void);

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

	virtual std::string getFullName(void) const;

	virtual std::string getLocalName(void) const;

	virtual variable::Type getType(void) const;

	bool isGlobal(void) const;

	virtual void assign(const variable* sc);

	void setParent(variable* parent);

	variable* getParent(void) const;

	unsigned int getVariableId(void) const;

	/**********************************************************/

	virtual void print(void) const;

	virtual void printTexada(void) const;

	virtual void printHexadecimal(void) const;

	/************************************************************/

	virtual size_t getSizeOf(void) const;

	size_t getOffset(void) const;

	size_t getEndOffset(void) const;

	void setPayload(payload* payLoad);
	
	payload* getPayload(void) const;

	/************************************************************/

	void addRawBytes(size_t size);

	void addField(const std::string& name, variable* var);

	void _addVariable(variable* subVar);

	void _rmVariable(const variable* var);

	bool hasVariables(void) const;

	std::list<variable*> getVariables(void) const;

	channel* getChannel(const std::string& name) const;

	void clearVariables(void);

	//std::list<variable*> addVariables(const varSymNode* sym);

	//std::list<variable*> createVariables(const varSymNode* sym);

	//variable* addVariable(const varSymNode* varSym);

	variable* getVariable(const std::string& name) const;

	template <typename T> T getTVariable(const std::string& name) const {
		std::map<std::string, variable*>::const_iterator resIt = varMap.find(name);
		if(resIt != varMap.cend())
			return dynamic_cast<T>(resIt->second);
	
		return parent? parent->getTVariable<T>(name) : nullptr;
	}

	template <typename T> std::list<T> getTVariables(void) const {
		std::list<T> res;
		for(auto var : varList) {
			auto varT = dynamic_cast<T>(var);
			if(varT != nullptr)
				res.push_back(varT);
		}
		return res;
	}

	std::map<std::string, variable*> getVariablesMap(void) const;


	unsigned long hash(void) const;

	/*********************************************************/

	static Type getVarType(symbol::Type type);

	static unsigned int vidCounter;


protected:
	std::string name;
	variable* parent;
	unsigned int vid;
	Type varType;
	size_t rawBytes;
	std::map<std::string, variable*> varMap;
	std::list<variable*> varList;
	size_t sizeOf;
	size_t offset;
	payload* payLoad;
	bool isHidden;
};

#endif