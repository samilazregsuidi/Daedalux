#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include <vector>
#include <map>

typedef char byte;
typedef unsigned char ubyte;

class payload;

#include "symbols.hpp"

class varSymNode;

class expr;
class exprArgList;
class exprRArgList;

class scalarInt;

class variable {
public:

	friend class variableDecorator;

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
		V_STACK,
		V_QUEUE,
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
		V_STRUCT,	// Type of variable is a user type (basically, a T_TDEF record is being used as the type): utype points to the type record
		V_NEVER,	// Never claim
		V_PROG,
		V_COMP_S,

		V_VARIANT
	};

	variable(Type type, const std::string& name = std::string());

	variable(const variable& other);

	virtual variable* deepCopy(void) const = 0;

	virtual ~variable();

	/****************************************************/

	virtual void init(void);

	virtual bool operator == (const variable* other) const;

	virtual bool operator != (const variable* other) const;

	virtual variable* operator=(const variable* other);

	//virtual variable* operator=(const argList& other);

	/****************************************************/

	template<typename T = variable*> T get(const std::string& name) const {
		auto res = dynamic_cast<T>(getVariableImpl(name));
		if(res == nullptr)
			throw std::runtime_error("Invalid cast");
		return res;
	}

	template<typename T = variable*> T get(size_t index) const {
		auto res = dynamic_cast<T>(varList[index]);
		if(res == nullptr)
			throw std::runtime_error("Invalid cast");
		return res;
	}

	template<typename T> std::list<T> getAll(void) const {
		auto res = std::list<T>();
		for(auto var : getVariables()){
			auto cast = dynamic_cast<T>(var);
			if(cast != nullptr)
				res.push_back(cast);
		}
		return res;
	}

	template<typename T> auto getValue(const std::string& name) const {
		auto res = get<T>(name);
		assert(res != nullptr);
		if constexpr(std::is_same<T, scalarInt*>::value) 
			return res->getIntValue();
		else
			return res->getValue();
	}

	/****************************************************/

	virtual void setGlobal(bool global);

	virtual void setPredef(bool predef);

	virtual void setHidden(bool hidden);

	virtual bool isGlobal(void) const;

	virtual bool isPredef(void) const;

	virtual bool isHidden(void) const;

	virtual std::string getFullName(void) const;

	virtual std::string getLocalName(void) const;

	virtual variable::Type getType(void) const;

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

	virtual std::map<std::string, variable*> getVariablesMap(void) const;

	virtual std::list<variable *> getVariablesList(void) const;

	virtual std::vector<variable*> getVariablesVector(void) const;

	//virtual channel* getChannel(const std::string& name) const;

	virtual void clearVariables(void);

	virtual void reset(void);

	//std::vector<variable*> addVariables(const varSymNode* sym);

	//std::vector<variable*> createVariables(const varSymNode* sym);

	//variable* addVariable(const varSymNode* varSym);

	virtual unsigned long hash(void) const;

	virtual float delta(const variable* v2) const;

	virtual void printDelta(const variable* v2) const;
	
	virtual std::list<variable*> getDelta(const variable* v2) const;

	bool isSame(const variable* other) const;

	/*********************************************************/

protected:
	virtual variable* getVariableImpl(const std::string& name) const;

	virtual variable* getVariableDownScoping(const std::string& name) const;

public:
	static Type getVarType(symbol::Type type);

	static unsigned int vidCounter;


protected:
	std::string name;
	variable* parent;
	unsigned int vid;
	Type varType;
	size_t rawBytes;
	std::map<std::string, variable*> varMap;
	std::vector<variable*> varList;
	//size_t sizeOf;
	size_t offset;
	payload* payLoad;
	bool hidden;
	bool predef;
	bool global;
};

#endif