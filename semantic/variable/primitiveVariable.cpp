#include "primitiveVariable.hpp"

#include "payload.hpp"
#include "symbols.hpp"
#include "constExpr.hpp"

primitiveVariable::primitiveVariable(const varSymNode* const varSym, variable* parent, unsigned int index)
	: variable(variable::getVarType(varSym->getType()), parent
	, std::string(varSym? (varSym->getBound() > 1? varSym->getName() + "["+std::to_string(index)+"]" : varSym->getName()) : ""))
	, varSym(varSym)
	, index(index)
{
	//assert(varSym && (varSym->getType() == symbol::T_INT || varSym->getType() == symbol::T_BIT || varSym->getType() == symbol::T_BYTE || varSym->getType() == symbol::T_SHORT));

	sizeOf += varSym->getTypeSize();
}

primitiveVariable::primitiveVariable(Type varType, variable* parent, unsigned int index) 
	: variable(varType, parent)
	, varSym(nullptr)
	, index(index)
{
	//assert(varSym && (varSym->getType() == symbol::T_INT || varSym->getType() == symbol::T_BIT || varSym->getType() == symbol::T_BYTE || varSym->getType() == symbol::T_SHORT));
	assert(false);

	if(index > 0)
		name += "["+std::to_string(index)+"]";

	sizeOf += varSym->getTypeSize();
}

primitiveVariable::primitiveVariable(const primitiveVariable& other)
	: variable(other)
	, varSym(other.varSym)
	, index(other.index)
{
}

primitiveVariable::primitiveVariable(const primitiveVariable* other)
	: variable(other)
	, varSym(other->varSym)
	, index(other->index)
{
}

variable* primitiveVariable::deepCopy(void) const {
	return new primitiveVariable(this);
}

void primitiveVariable::init(void) {

	auto initExpr = varSym->getInitExpr();

	if(initExpr) {
		auto initExprConst = dynamic_cast<exprConst*>(initExpr);
		//init expr should be const and const only!
		assert(initExprConst);
		setValue(initExprConst->getCstValue());
	}

	variable::init();
}

int primitiveVariable::operator = (const primitiveVariable& rvalue) {
	int res = rvalue.getValue(); 
	setValue(res);
	return res;
}

int primitiveVariable::operator ++ (void) {
	setValue(getValue()+1);
	return getValue();
}

int primitiveVariable::operator -- (void) {
	setValue(getValue()-1);
	return getValue();
}

int primitiveVariable::operator ++ (int) {
	auto temp = getValue();
	setValue(getValue()+1);
	return temp;
}

int primitiveVariable::operator -- (int) {
	auto temp = getValue();
	setValue(getValue()-1);
	return temp;
}

bool primitiveVariable::operator == (const primitiveVariable& other) const {
	return getValue() == other.getValue();
}

bool primitiveVariable::operator != (const primitiveVariable& other) const {
	return getValue() == other.getValue();
}

void primitiveVariable::setValue(int value) {
	assert(value >= varSymNode::getLowerBound(varSym->getType()) && value <= varSymNode::getUpperBound(varSym->getType()));
	getPayload()->setValue(offset, value, getType());
}
	
int primitiveVariable::getValue(void) const {
	auto value = getPayload()->getValue(offset, getType());
	assert(value >= varSymNode::getLowerBound(varSym->getType()) && value <= varSymNode::getUpperBound(varSym->getType()));
	return value;
}

void primitiveVariable::print(void) const {
	auto value = getPayload()->getValue(offset, getType());
	printf("0x%-4lx:   %-23s = %d\n", offset, getLocalName().c_str(), value);

	variable::print();
}

void primitiveVariable::printTexada(void) const {
	if(varSym->isPredefined())
		return;

	auto value = getPayload()->getValue(offset, getType());
	printf("%s = %d\n", getLocalName().c_str(), value);

	variable::printTexada();
}

/*************************************************************************************************/

constVar::constVar(int value, variable::Type type, int lineNb)
	: primitiveVariable(type)
	, value(value)
	, lineNb(lineNb)
{
	assert(false);//sizeOf += 
}

void constVar::setValue(int value) {
	value;
	assert(false);
}

int constVar::getValue(void) const {
	return value;
}

int constVar::operator = (const primitiveVariable& rvalue) {
	rvalue;
	assert(false);
}

int constVar::operator ++ (void) {
	assert(false);
}

int constVar::operator -- (void) {
	assert(false);
}

int constVar::operator ++ (int) {
	assert(false);
}

int constVar::operator -- (int) {
	assert(false);
}

variable* constVar::deepCopy(void) const {
	constVar* copy = new constVar(*this);
	//warning shared payload! 
	return copy;
}

/******************************************************************************************************/

#include "process.hpp"

PIDVar::PIDVar(variable* parent, const pidSymNode* sym, unsigned int bound) 
	: primitiveVariable(sym, parent, bound)
	, ref(nullptr)
{}

variable* PIDVar::deepCopy(void) const{
	variable* copy = new PIDVar(*this);
	return copy;
}

process* PIDVar::getRefProcess(void) const {
	return ref;
}
	
void PIDVar::setRefProcess(process* newRef) {
	ref = newRef;
	getPayload()->setValue<byte>(offset, newRef->getPid());
}

void PIDVar::assign(const variable* sc) {
	variable::assign(sc);
	if(ref) {
		ref = getTVariable<process*>(ref->getLocalName());
		assert(ref);
	}
}
