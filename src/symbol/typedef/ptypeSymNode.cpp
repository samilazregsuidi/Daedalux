#include "ptypeSymNode.hpp"
#include "symTabVisitor.hpp"
#include "constExpr.hpp"
#include "stmnt.hpp"

seqSymNode::seqSymNode(Type type, const std::string& name, int lineNb, stmnt* block)
	: symbol(type, name, lineNb)
{
	this->block = block;
}
	

stmnt* seqSymNode::getBlock(void) const {
	return block;
}

symTable* seqSymNode::getSymTable(void) const {
	return block->getLocalSymTab();
}

symTable* seqSymNode::getSubSymTable(void) const {
	return block->getLocalSymTab();
}

seqSymNode::operator std::string(void) const {
	return name + "{\n" + (block? std::string(*block) : "") + "\n}";
}

/****************************************************************************************************************/

ptypeSymNode::ptypeSymNode(const std::string& name, exprConst* active, const std::list<varSymNode*>& args, stmnt* block, int lineNb)
	: seqSymNode(symbol::T_PTYPE, name, lineNb, block)
{
	this->args = args;
	this->active = active;
}

ptypeSymNode::~ptypeSymNode() { 
	if (active) 
		delete active;
}

std::string ptypeSymNode::getTypeName(void) const {
	return "proctype";
}

std::string ptypeSymNode::getBasicTypeName(void) const {
	return "proctype";
}

int ptypeSymNode::getTypeSize(void) const {
	return 0;
}

//void resolveVariables(symbol* globalSymTab, const mTypeList* mTypes, varSymNode* localSymTab = nullptr, symbol* subFieldSymTab = nullptr) override ;
exprConst* ptypeSymNode::getActiveExpr(void) const {
	return active;
}

const std::list<varSymNode*>& ptypeSymNode::getArgs(void) const {
	return args;
}

ptypeSymNode::operator std::string(void) const {
	assert(active);
	std::string res = "";
	if (active->getCstValue() >= 1){
		res += "active";
	}
	if (active->getCstValue() > 1) {
		res += " [" + std::string(*active) + "]";
	}

	res += " proctype " + name + "(";
	unsigned int i = 0;
	for(auto it : args)
		res += it->getTypeName() + " " + it->getName() + (++i != args.size() ? "; " : "");

	res += "){\n" + (block? stmnt::string(block) : "") + "}\n";

	return res;
}

void ptypeSymNode::acceptVisitor(symTabVisitor *visitor) {
	visitor->visitProc(this);
}

void ptypeSymNode::acceptVisitor(symTabConstVisitor *visitor) const{
	visitor->visitProc(this);
}

/********************************************************************************************************/

initSymNode::initSymNode(int lineNb, stmnt* block)
	: seqSymNode(symbol::T_INIT, "init", lineNb, block)
{}

std::string initSymNode::getTypeName(void) const {
	return "init";
}

std::string initSymNode::getBasicTypeName(void) const {
	return "init";
}

int initSymNode::getTypeSize(void) const {
	return 1;
}

void initSymNode::acceptVisitor(symTabVisitor *visitor) {
	visitor->visitInit(this);
}

void initSymNode::acceptVisitor(symTabConstVisitor *visitor) const{
	visitor->visitInit(this);
}

/********************************************************************************************************/

neverSymNode::neverSymNode(int lineNb, stmnt* block)
	: seqSymNode(symbol::T_INIT, "never", lineNb, block)
{}

std::string neverSymNode::getTypeName(void) const {
	return "never";
}

std::string neverSymNode::getBasicTypeName(void) const {
	return "never";
}

int neverSymNode::getTypeSize(void) const {
	return 1;
}

void neverSymNode::acceptVisitor(symTabVisitor *visitor) {
	visitor->visitNever(this);
}

void neverSymNode::acceptVisitor(symTabConstVisitor *visitor) const{
	visitor->visitNever(this);
}

