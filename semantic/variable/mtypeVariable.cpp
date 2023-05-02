#include "mtypeVariable.hpp"

#include "varExpr.hpp"

mtypeVar::mtypeVar(const mtypeSymNode* sym, unsigned int index)
	: primitiveVariable(sym, index)
{}

void mtypeVar::init(void) {

	auto initExpr = varSym->getInitExpr();

	if(initExpr) {
		
		//its a cmtype value...
		assert(initExpr->getType() == astNode::E_EXPR_VAR);

		auto sym = dynamic_cast<exprVar*>(initExpr)->getFinalSymbol();
		assert(sym && sym->getType() == symbol::T_CMTYPE);

		setValue(dynamic_cast<const cmtypeSymNode*>(sym)->getIntValue());
	}
}

int mtypeVar::operator ++ (void) {
	assert(false);
}

int mtypeVar::operator -- (void) {
	assert(false);
}

int mtypeVar::operator ++ (int) {
	assert(false);
}

int mtypeVar::operator -- (int) {
	assert(false);
}

variable* mtypeVar::deepCopy(void) const {
	mtypeVar* copy = new mtypeVar(*this);
	//warning shared payload! 
	return copy;
}

mtypeVar::operator std::string(void) const {
	char buffer[128];
	auto value = getValue();
	if(value) {
		auto def = dynamic_cast<const mtypeSymNode*>(varSym)->getMTypeDef();
		if(def) {
			auto mtypestr = def->getCmtypeSymNodeName(value);
			sprintf(buffer, "0x%-4lx:   %-23s = %s\n", getOffset(), getFullName().c_str(), mtypestr.c_str());
		} else {
			sprintf(buffer, "0x%-4lx:   %-23s = %d\n", getOffset(), getFullName().c_str(), value);
		}
	} else {
		sprintf(buffer, "0x%-4lx:   %-23s = nil\n", getOffset(), getFullName().c_str());
	}
	return buffer;
}

void mtypeVar::print(void) const {
	printf("%s", std::string(*this).c_str());
}

void mtypeVar::printTexada(void) const {
	if(varSym->isPredefined())
		return;
	auto value = getValue();
	if(value) {
		auto def = dynamic_cast<const mtypeSymNode*>(varSym)->getMTypeDef();
		auto mtypestr = def->getCmtypeSymNodeName(value);
		printf("%s = %s\n", getFullName().c_str(), mtypestr.c_str());
	} else {
		printf("%s = nil\n", getFullName().c_str());
	}
}

/******************************************************************************************************/

cmtypeVar::cmtypeVar(const cmtypeSymNode* sym) 
	: primitiveVariable(sym)
{}

void cmtypeVar::setValue(int value) {
	value;
	assert(false);
}
	
int cmtypeVar::getValue(void) const {
	return dynamic_cast<const cmtypeSymNode*>(varSym)->getIntValue();
}

variable* cmtypeVar::deepCopy(void) const {
	cmtypeVar* copy = new cmtypeVar(*this);
	//warning shared payload! 
	return copy;
}

int cmtypeVar::operator = (const primitiveVariable& rvalue) {
	rvalue;
	assert(false);
}

int cmtypeVar::operator ++ (void) {
	assert(false);
}

int cmtypeVar::operator -- (void) {
	assert(false);
}

int cmtypeVar::operator ++ (int) {
	assert(false);
}

int cmtypeVar::operator -- (int) {
	assert(false);
}

cmtypeVar::operator std::string() const {
	return std::string();
	//assert(false);
}

void cmtypeVar::print(void) const {
	//assert(false);
}

void cmtypeVar::printTexada(void) const {
}

/******************************************************************************************************/