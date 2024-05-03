#include "variableDecorator.hpp"

#include <iterator>

#include "payload.hpp"

#include "argExpr.hpp"
#include "constExpr.hpp"
#include "varExpr.hpp"

variableDecorator::variableDecorator(variable* wrapee)
	: variable(wrapee->getType())
	, wrappee(wrapee)
{
}

variableDecorator::variableDecorator(const variableDecorator& other)
	: variable(other.getType())
	, wrappee(other.wrappee)
{
}

variableDecorator::variableDecorator(const variableDecorator* other)
	: variable(other->getType())
	, wrappee(nullptr)
{
	wrappee = other->wrappee->deepCopy();
}

variableDecorator::~variableDecorator() {
	delete wrappee;
}

variableDecorator::Type variableDecorator::getType(void) const {
	return wrappee->getType();
}

void variableDecorator::assign(const variable* sc) {
	wrappee->assign(sc);
}

void variableDecorator::init(void) {
	wrappee->init();
}

bool variableDecorator::isGlobal(void) const {
	return wrappee->isGlobal();
}

bool variableDecorator::operator == (const variable* other) const {
	auto cast = dynamic_cast<const variableDecorator*>(other);
	if(cast)
		return *wrappee == cast->wrappee;
	return *wrappee == other;
}

bool variableDecorator::operator != (const variable* other) const {
	return !(*this == other);
}

void variableDecorator::setParent(variable* parent) {
	wrappee->setParent(parent);
}

variable* variableDecorator::getParent(void) const {
	return wrappee->getParent();
}

std::string variableDecorator::getFullName(void) const {
	return wrappee->getFullName();
}

std::string variableDecorator::getLocalName(void) const {
	return wrappee->getLocalName();
}

unsigned int variableDecorator::getVariableId(void) const {
	return wrappee->getVariableId();
}

void variableDecorator::_addVariable(variable* var) {
	wrappee->_addVariable(var);
}

void variableDecorator::_rmVariable(const variable* var) {
	wrappee->_rmVariable(var);
}

bool variableDecorator::hasVariables(void) const {
	return wrappee->hasVariables();
}

std::list<variable*> variableDecorator::getVariables(void) const {
	return wrappee->getVariables();
}

variableDecorator::operator std::string() const {
	return wrappee->operator std::string();
}

void variableDecorator::print(void) const {
	wrappee->print();
}

void variableDecorator::printTexada(void) const {
	wrappee->printTexada();
}

void variableDecorator::printCSV(std::ostream &out) const {
	wrappee->printCSV(out);
}

void variableDecorator::printCSVHeader(std::ostream &out) const {
	wrappee->printCSVHeader(out);
}

void variableDecorator::printHexadecimal(void) const {
	wrappee->printHexadecimal();
}

void variableDecorator::setPayload(payload* newPayLoad) {
	wrappee->setPayload(newPayLoad);
}

payload* variableDecorator::getPayload(void) const {
	return wrappee->getPayload();
}

unsigned long variableDecorator::hash(void) const {
	return wrappee->hash();
}

size_t variableDecorator::getOffset(void) const {
	return wrappee->getOffset();
}

size_t variableDecorator::getEndOffset(void) const {
	return wrappee->getEndOffset();
}

void variableDecorator::addRawBytes(size_t size) {
	wrappee->addRawBytes(size);
}

/*std::list<variable*> variableDecorator::addVariables(const varSymNode* sym) {
	assert(sym);

	std::list<variable*> res;

	switch(sym->getType())
	{
	case symbol::T_NA:
		assert(false);
	case symbol::T_BOOL:
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new boolVar(dynamic_cast<const boolSymNode*>(sym), i);
			_addVariable(var);
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
			auto var = new primitiveVariable(sym, i);
			_addVariable(var);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_UNSGN: 	// not supported yet
		assert(false);
	case symbol::T_MTYPE:
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new mtypeVar(dynamic_cast<const mtypeSymNode*>(sym), i);
			_addVariable(var);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_CLOCK:	// dense time clock
	case symbol::T_MTYPE_DEF:
		assert(false);
	case symbol::T_CMTYPE:
	{
		auto var = new cmtypeVar(dynamic_cast<const cmtypeSymNode*>(sym));
		_addVariable(var);
		res.push_back(var);
		return res;
	}
	//case symbol::T_FEAT,
	//case symbol::T_UFEAT,
	case symbol::T_UTYPE:	// Type of variable is a user type (basically, a case symbol::T_TDEF record is being used as the type): utype points to the type record
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new utypeVar(dynamic_cast<const utypeSymNode*>(sym), i);
			_addVariable(var);
			res.push_back(var);
		}
		return res;
	}
		// "Special" types:
	case symbol::T_CHAN:		// Channel: capacity used; children denote message fields
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new channel(dynamic_cast<const chanSymNode*>(sym), i);
			_addVariable(var);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_CID:		// Channel reference; capacity and children are not used.
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new CIDVar(dynamic_cast<const cidSymNode*>(sym), i);
			_addVariable(var);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_PID:
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new PIDVar(dynamic_cast<const pidSymNode*>(sym), i);
			_addVariable(var);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_TDEF:		// Type definition: children denote fields of type
	case symbol::T_INIT:
	case symbol::T_PTYPE:		// ProcType: fsm field used; bound denotes the number of initially active processes
	case symbol::T_INLINE:
	case symbol::T_NEVER:	// Never claim
		assert(false);
	default:
		assert(false);
	}
	return res;
}*/
/*
std::list<variable*> variableDecorator::createVariables(const varSymNode* sym) {
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

variable* variableDecorator::getVariableImpl(const std::string& name) const {
	return wrappee->getVariableImpl(name);
}

std::map<std::string, variable*> variableDecorator::getVariablesMap(void) const {
	return wrappee->getVariablesMap();
}

std::list<variable*> variableDecorator::getVariablesList(void) const {
	return wrappee->getVariablesList();
}

size_t variableDecorator::getSizeOf(void) const {
	return wrappee->getSizeOf();
}

void variableDecorator::clearVariables(void) {
	return wrappee->clearVariables();
}

/*************************************************************************************************/