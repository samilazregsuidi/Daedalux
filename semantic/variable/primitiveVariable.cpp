#include "primitiveVariable.hpp"

#include "payload.hpp"
#include "symbols.hpp"
#include "constExpr.hpp"

primitiveVariable::primitiveVariable(const varSymNode* const varSym, unsigned int index)
	: variable(variable::getVarType(varSym->getType())
	, std::string(varSym? (varSym->getBound() > 1? varSym->getName() + "["+std::to_string(index)+"]" : varSym->getName()) : ""))
	, varSym(varSym)
	, index(index)
{
	//assert(varSym && (varSym->getType() == symbol::T_INT || varSym->getType() == symbol::T_BIT || varSym->getType() == symbol::T_BYTE || varSym->getType() == symbol::T_SHORT));
}

primitiveVariable::primitiveVariable(Type varType, unsigned int index) 
	: variable(varType)
	, varSym(nullptr)
	, index(index)
{
	//assert(varSym && (varSym->getType() == symbol::T_INT || varSym->getType() == symbol::T_BIT || varSym->getType() == symbol::T_BYTE || varSym->getType() == symbol::T_SHORT));
	assert(false);

	if(index > 0)
		name += "["+std::to_string(index)+"]";
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

bool primitiveVariable::isGlobal(void) const {
	return varSym->isGlobal();
}

size_t primitiveVariable::getSizeOf(void) const {
	return varSym->getTypeSize();
}

int primitiveVariable::operator = (const primitiveVariable& rvalue) {
	int res = rvalue.getValue(); 
	setValue(res);
	return res;
}

int primitiveVariable::operator ++ (void) {
	auto temp = getValue();
	auto bound = varSymNode::getUpperBound(varSym->getType());
	if(temp < bound) {
		setValue(temp + 1);
		return temp + 1;
	}
	return temp;
}

int primitiveVariable::operator -- (void) {
	auto temp = getValue();
	if(temp - 1  >= varSymNode::getLowerBound(varSym->getType())) {
		setValue(temp - 1);
		return temp - 1;
	}
	return temp;
}

int primitiveVariable::operator ++ (int) {
	auto temp = getValue();
	if(temp + 1 <= varSymNode::getUpperBound(varSym->getType()))
		setValue(temp + 1);
	return temp;
}

int primitiveVariable::operator -- (int) {
	auto temp = getValue();
	if(temp - 1  >= varSymNode::getLowerBound(varSym->getType()))
		setValue(temp - 1);
	return temp;
}

bool primitiveVariable::operator == (const primitiveVariable& other) const {
	return getValue() == other.getValue();
}

bool primitiveVariable::operator != (const primitiveVariable& other) const {
	return getValue() == other.getValue();
}

void primitiveVariable::setValue(int value) {
	assert(getPayload());
	assert(value >= varSymNode::getLowerBound(varSym->getType()));
	assert(value <= varSymNode::getUpperBound(varSym->getType()));
	getPayload()->setValue(getOffset(), value, getType());
}
	
int primitiveVariable::getValue(void) const {
	assert(getPayload());
	auto value = getPayload()->getValue(getOffset(), getType());
	//assert(value >= varSymNode::getLowerBound(varSym->getType()) && value <= varSymNode::getUpperBound(varSym->getType()));
	return value;
}

void primitiveVariable::print(void) const {
	assert(getPayload());
	auto value = getPayload()->getValue(getOffset(), getType());
	printf("0x%-4lx:   %-23s = %d\n", getOffset(), getFullName().c_str(), value);

	variable::print();
}

void primitiveVariable::printTexada(void) const {
	assert(getPayload());
	if(varSym->isPredefined())
		return;

	auto value = getPayload()->getValue(getOffset(), getType());
	printf("%s = %d\n", getFullName().c_str(), value);

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
	value = value;
	assert(false);
}

int constVar::getValue(void) const {
	return value;
}

int constVar::operator = (const primitiveVariable& rvalue) {
	value = rvalue.getValue();
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

PIDVar::PIDVar(const pidSymNode* sym, unsigned int bound) 
	: primitiveVariable(sym, bound)
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
	getPayload()->setValue<byte>(getOffset(), newRef->getPid());
}

void PIDVar::assign(const variable* sc) {
	variable::assign(sc);
	if(ref) {
		ref = getTVariable<process*>(ref->getLocalName());
		assert(ref);
	}
}
