#include "variable.hpp"

#include <iterator>

#include "payload.hpp"
#include "process.hpp"

#include "channel.hpp"
#include "utypeVariable.hpp"
#include "boolVariable.hpp"
#include "mtypeVariable.hpp"

#include "argExpr.hpp"
#include "constExpr.hpp"
#include "varExpr.hpp"

unsigned int padding(const varSymNode* varSym){
	switch(varSym->getType()){
		case symbol::T_BIT:
		case symbol::T_BOOL:
		case symbol::T_BYTE:
		case symbol::T_CHAN:
		case symbol::T_MTYPE:
		case symbol::T_PID:
			return 1;
		
		case symbol::T_SHORT:
			return 2;

		case symbol::T_CID:
		case symbol::T_INT:
			return 4;
		
		case symbol::T_UTYPE:
			return padding(*dynamic_cast<const utypeSymNode*>(varSym)->getUType()->getFields().cbegin());

		default:
			assert(false);
			return -1;
	}
	assert(false);
	return -1;
}

variable::Type variable::getVarType(symbol::Type type) {
	switch(type){
		case symbol::T_BIT:
			return V_BIT;
		case symbol::T_BOOL:
			return V_BOOL;
		case symbol::T_BYTE:
			return V_BYTE;
		case symbol::T_CHAN:
			return V_CHAN;
		case symbol::T_MTYPE:
			return V_MTYPE;
		case symbol::T_PID:
			return V_PID;
		case symbol::T_SHORT:
			return V_SHORT;
		case symbol::T_CID:
			return V_CID;
		case symbol::T_INT:
			return V_INT;
		case symbol::T_UTYPE:
			return V_UTYPE;
		default:
			assert(false);
			return V_NA;
	}
	assert(false);
	return V_NA;
}

unsigned int variable::vidCounter = 0;

variable::variable(Type varType, variable* parent, const std::string& name)
	: name(name)
	, parent(parent)
	, vid(++vidCounter)
	, varType(varType)
	, rawBytes(0)
	, sizeOf(0)
	, offset(0)
	, payLoad(nullptr)
	, isHidden(false)
{
	if(parent) {
		offset = parent->getEndOffset();
		payLoad = parent->getPayload();
		parent->_addVariable(this);
	}
}

variable::variable(const variable& other)
	: name(other.name)
	, parent(other.parent)
	, vid(other.vid)
	, rawBytes(other.rawBytes)
	, sizeOf(other.sizeOf)
	, offset(other.offset)
	, payLoad(other.payLoad)
	, isHidden(other.isHidden)
{
}

variable::variable(const variable* other)
	: variable(*other)
{
	payLoad = nullptr;
	clearVariables();

	for(auto subVar : other->getVariables()){
		_addVariable(subVar->deepCopy());
	}

	for(auto subVar : getVariables())
		subVar->assign(this);
}

variable::~variable() {
	for(auto var : varList)
		delete var;

	if(parent)
		parent->_rmVariable(this);
}

variable::Type variable::getType(void) const {
	return varType;
}

void variable::assign(const variable* sc) {
	if(parent) {
		parent = sc->getVariable(parent->getLocalName());
		assert(parent);
	}

	if(hasVariables()){
		std::list<variable*> newFields;
		for(auto varSubField : getVariables()) {
			auto field = sc->getVariable(varSubField->getLocalName());
			assert(field);
			newFields.push_back(field);
		}
		varList = newFields;
	}
}

void variable::init(void) {

	if(!payLoad) {
		setPayload(new payload(getSizeOf()));
	}

	for(auto var : varList)
		var->init();
}

/*bool variable::operator == (const variable& other) const {
	return getValue() == other.getValue();
}

bool variable::operator != (const variable& other) const {
	return getValue() == other.getValue();
}*/

void variable::setParent(variable* parent) {
	this->parent = parent;
}

variable* variable::getParent(void) const {
	return parent;
}

std::string variable::getFullName(void) const {
	return parent? parent->getLocalName() + "." + getLocalName() : getLocalName();
}

std::string variable::getLocalName(void) const {
	return name;
}

unsigned int variable::getVariableId(void) const {
	return vid;
}

void variable::_addVariable(variable* var) {
	var->setParent(this);
	auto varName = var->getLocalName();
	varMap[varName] = var;
	varList.push_back(var);
}

void variable::_rmVariable(const variable* var) {
	varMap.erase(var->getLocalName());
	varList.erase(std::find(varList.begin(), varList.end(), var));
}

bool variable::hasVariables(void) const {
	return getVariables().size() > 0;
}

std::list<variable*> variable::getVariables(void) const {
	return varList;
}

void variable::print(void) const {
	for(auto var : varList)
		var->print();
}

void variable::printTexada(void) const {
	for(auto var : varList)
		var->printTexada();
}

void variable::printHexadecimal(void) const {
	payLoad->printHexadecimal(offset, getSizeOf());
}


void variable::setPayload(payload* newPayLoad) {
	assert(!payLoad);
	payLoad = newPayLoad;

	for(auto var : varList)
		var->setPayload(newPayLoad);
}

payload* variable::getPayload(void) const {
	return payLoad;
}

unsigned long variable::hash(void) const {
	return payLoad->hash(offset, getSizeOf());
}

size_t variable::getOffset(void) const {
	return offset;
}

size_t variable::getEndOffset(void) const {
	auto subSize = 0;
	for(auto var : varList)
		subSize += var->getSizeOf();
	return offset + sizeOf + subSize;
}

void variable::addRawBytes(size_t size) {
	rawBytes += size;
	sizeOf += size;
}

std::list<variable*> variable::addVariables(const varSymNode* sym) {
	assert(sym);

	std::list<variable*> res;

	switch(sym->getType())
	{
	case symbol::T_NA:
		assert(false);
	case symbol::T_BOOL:
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new boolVar(this, dynamic_cast<const boolSymNode*>(sym), i);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_BIT:
	case symbol::T_BYTE:
	case symbol::T_SHORT:
	case symbol::T_INT:
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new primitiveVariable(sym, this, i);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_UNSGN: 	// not supported yet
		assert(false);
	case symbol::T_MTYPE:
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new mtypeVar(this, dynamic_cast<const mtypeSymNode*>(sym), i);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_CLOCK:	// dense time clock
	case symbol::T_MTYPE_DEF:
		assert(false);
	case symbol::T_CMTYPE:
	{
		auto var = new cmtypeVar(this, dynamic_cast<const cmtypeSymNode*>(sym));
		res.push_back(var);
		return res;
	}
	//case symbol::T_FEAT,
	//case symbol::T_UFEAT,
	case symbol::T_UTYPE:	// Type of variable is a user type (basically, a case symbol::T_TDEF record is being used as the type): utype points to the type record
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new utypeVar(this, dynamic_cast<const utypeSymNode*>(sym), i);
			res.push_back(var);
		}
		return res;
	}
		// "Special" types:
	case symbol::T_CHAN:		// Channel: capacity used; children denote message fields
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new channel(this, dynamic_cast<const chanSymNode*>(sym), i);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_CID:		// Channel reference; capacity and children are not used.
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new CIDVar(this, dynamic_cast<const cidSymNode*>(sym), i);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_PID:
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new PIDVar(this, dynamic_cast<const pidSymNode*>(sym), i);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_TDEF:		// Type definition: children denote fields of type
	case symbol::T_INIT:
	case symbol::T_PROC:		// ProcType: fsm field used; bound denotes the number of initially active processes
	case symbol::T_INLINE:
	case symbol::T_NEVER:	// Never claim
		assert(false);
	default:
		assert(false);
	}
	return res;
}
/*
std::list<variable*> variable::createVariables(const varSymNode* sym) {
	assert(sym);

	std::list<variable*> res;

	switch(sym->getType())
	{
	case symbol::T_NA:
		assert(false);
	case symbol::T_BOOL:
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new boolVar(this, dynamic_cast<const boolSymNode*>(sym), i);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_BIT:
	case symbol::T_BYTE:
	case symbol::T_SHORT:
	case symbol::T_INT:
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new primitiveVariable(sym, this, i);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_UNSGN: 	// not supported yet
		assert(false);
	case symbol::T_MTYPE:
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new mtypeVar(this, dynamic_cast<const mtypeSymNode*>(sym), i);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_CLOCK:	// dense time clock
	case symbol::T_MTYPE_DEF:
		assert(false);
	case symbol::T_CMTYPE:
	{
		auto var = new cmtypeVar(this, dynamic_cast<const cmtypeSymNode*>(sym));
		res.push_back(var);
		return res;
	}
	//case symbol::T_FEAT,
	//case symbol::T_UFEAT,
	case symbol::T_UTYPE:	// Type of variable is a user type (basically, a case symbol::T_TDEF record is being used as the type): utype points to the type record
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new utypeVar(this, dynamic_cast<const utypeSymNode*>(sym), i);
			res.push_back(var);
		}
		return res;
	}
		// "Special" types:
	case symbol::T_CHAN:		// Channel: capacity used; children denote message fields
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new channel(this, dynamic_cast<const chanSymNode*>(sym), i);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_CID:		// Channel reference; capacity and children are not used.
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new CIDVar(this, dynamic_cast<const cidSymNode*>(sym), i);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_PID:
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new PIDVar(this, dynamic_cast<const pidSymNode*>(sym), i);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_TDEF:		// Type definition: children denote fields of type
	case symbol::T_INIT:
	case symbol::T_PROC:		// ProcType: fsm field used; bound denotes the number of initially active processes
	case symbol::T_INLINE:
	case symbol::T_NEVER:	// Never claim
		assert(false);
	default:
		assert(false);
	}
	return res;
}
*/
variable* variable::getVariable(const std::string& name) const {
	std::map<std::string, variable*>::const_iterator resIt = varMap.find(name);
	if(resIt != varMap.cend())
		return resIt->second;
	
	return parent? parent->getVariable(name) : nullptr;
}

channel* variable::getChannel(const std::string& name) const {
	
	auto var = getVariable(name);
	
	if(!var) return nullptr;

	channel* chan = nullptr;
	if(var->getType() == variable::V_CID){
		chan = dynamic_cast<CIDVar*>(var)->getRefChannel();
		assert(chan);
	} else {
		assert(var->getType() == variable::V_CHAN);
		chan = dynamic_cast<channel*>(var);
	}
	assert(chan);
	return chan;
}

std::map<std::string, variable*> variable::getVariablesMap(void) const {
	return varMap;
}


size_t variable::getSizeOf(void) const {
	size_t size = sizeOf;

	for(auto var : varList)
		size += var->getSizeOf();

	return size;
}

void variable::clearVariables(void) {
	varList.clear();
	varMap.clear();
}

/*************************************************************************************************/