#include "sysSymNode.hpp"

#include "symTable.hpp"

sysSymNode::sysSymNode(int lineNb, const std::string& name, unsigned int bound, expr* init)
	: varSymNode(symbol::T_SYS, lineNb, name, bound, init)
{}

symTable* sysSymNode::getSubSymTable(void) const {
	assert(parent);
	auto it = parent;
	while(it) {
		if(parent->getNameSpace() == "global")
			return parent;
	}
	assert(false);
	return nullptr;
}

std::string sysSymNode::getBasicTypeName(void) const {
	return "system";
}

std::string sysSymNode::getTypeName(void) const {
	return getBasicTypeName();
}

int sysSymNode::getTypeSize(void) const {
	return 0;
}

sysSymNode::operator std::string(void) const {
	return getBasicTypeName() + " " + name + ";";
}

void sysSymNode::acceptVisitor(symTabVisitor* visitor) {

}

void sysSymNode::acceptVisitor(symTabConstVisitor* visitor) const {
	
}
