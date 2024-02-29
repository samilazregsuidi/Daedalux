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

class primitiveVariable;
class boolVar;

template <typename T> struct return_value {
	using type = T;
};

template <> struct return_value<primitiveVariable*> {
	using type = int;
};

template <> struct return_value<boolVar*> {
	using type = bool;
};

class variable {
public:

	enum Type {
		V_NA,
		V_BIT,
		V_BOOL,
		V_BYTE,
		V_PID,
		V_SHORT,
		V_USHORT,
		V_INT,
		V_UINT,
		V_LONG,
		V_ULONG,
		V_FLOAT,
		V_DOUBLE,
		V_UNSGN, 	// not supported yet
		V_MTYPE,
		V_CLOCK ,	// dense time clock - supports RT?
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
		V_PROG,
		V_COMP_S,

		V_VARIANT
	};

	template <Type type> struct bounds {
		static const int min;
		static const int max;
	};

	template <Type type> struct size {
		static const int value;
	};

	variable(Type type, const std::string& name = std::string());

	variable(const variable& other);
	
	variable(const variable* other);

	virtual variable* deepCopy(void) const = 0;

	virtual ~variable();

	/****************************************************/

	virtual void init(void);

	virtual bool operator == (const variable* other) const;

	virtual bool operator != (const variable* other) const;

	/****************************************************/

	virtual std::string getFullName(void) const;

	virtual std::string getLocalName(void) const;

	virtual variable::Type getType(void) const;

	virtual bool isGlobal(void) const;

	virtual void assign(const variable* sc);

	virtual void setParent(variable* parent);

	virtual variable* getParent(void) const;

	virtual unsigned int getVariableId(void) const;

	/**********************************************************/

	virtual operator std::string(void) const;

	virtual void print(void) const;

	virtual void printTexada(void) const;

	virtual void printCSV(std::ostream& out) const;

	virtual void printCSVHeader(std::ostream& out) const;

	virtual void printHexadecimal(void) const;

	/************************************************************/

	virtual size_t getSizeOf(void) const;

	virtual size_t getOffset(void) const;

	virtual size_t getEndOffset(void) const;

	virtual void setPayload(payload* payLoad);
	
	virtual payload* getPayload(void) const;

	/************************************************************/

	virtual void addRawBytes(size_t size);

	//virtual void addField(const std::string& name, variable* var);

	virtual void _addVariable(variable* subVar);

	virtual void _rmVariable(const variable* var);

	virtual bool hasVariables(void) const;

	virtual std::list<variable*> getVariables(void) const;

	virtual std::list<variable *> getAllVariables(void) const;

	virtual channel* getChannel(const std::string& name) const;

	virtual void clearVariables(void);

	virtual void reset(void);

	//std::list<variable*> addVariables(const varSymNode* sym);

	//std::list<variable*> createVariables(const varSymNode* sym);

	//variable* addVariable(const varSymNode* varSym);

	virtual variable* getVariable(const std::string& name) const;

	virtual variable* getVariableDownScoping(const std::string& name) const;

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

	template <class T> typename return_value<T>::type getValue(const std::string& name) const {
		auto var = getVariable(name);
		if(var != nullptr)
			return (dynamic_cast<T>(var))->getValue();
		assert(false);
	}

	virtual std::map<std::string, variable*> getVariablesMap(void) const;

	virtual unsigned long hash(void) const;

	virtual float delta(const variable* v2) const;

	virtual void printDelta(const variable* v2) const;
	
	virtual std::list<variable*> getDelta(const variable* v2) const;

	bool isSame(const variable* other) const;

	/*********************************************************/

	static Type getVarType(symbol::Type type);

	static size_t getUpperBound(variable::Type type);

	static size_t getLowerBound(variable::Type type);

	static unsigned int vidCounter;


public:
	std::string name;
	variable* parent;
	unsigned int vid;
	Type varType;
	size_t rawBytes;
	std::map<std::string, variable*> varMap;
	std::list<variable*> varList;
	//size_t sizeOf;
	size_t offset;
	payload* payLoad;
	bool isHidden;
	bool isPredef;
};

#endif