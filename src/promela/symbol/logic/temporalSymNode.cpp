#include "temporalSymNode.hpp"

#include "temporalExpr.hpp"
#include "expr.hpp"

ltlSymNode::ltlSymNode(const std::string& name, expr* formula, int lineNb)
	: symbol(symbol::T_LTL, name, lineNb)
	, formula(formula)
{
}

std::string ltlSymNode::getTypeName(void) const {
	return "ltl";
}

std::string ltlSymNode::getBasicTypeName(void) const {
	return "ltl";
}

int ltlSymNode::getTypeSize(void) const {
	return 0;
}

ltlSymNode::operator std::string(void) const {
	std::string res = "";

	res += getBasicTypeName() + " " + name + " {\n\t";
	res += std::string(*formula) + "\n}";

	return res;
}

void ltlSymNode::acceptVisitor(symTabVisitor* visitor) {}

void ltlSymNode::acceptVisitor(symTabConstVisitor* visitor) const {}

/*************************************************************************************************************************************/

bltlSymNode::bltlSymNode(const std::string& name, expr* formula, int lineNb)
	: symbol(symbol::T_BLTL, name, lineNb)
	, formula(formula)
{
}

std::string bltlSymNode::getTypeName(void) const {
	return "bltl";
}

std::string bltlSymNode::getBasicTypeName(void) const {
	return "bltl";
}

int bltlSymNode::getTypeSize(void) const {
	return 0;
}

bltlSymNode::operator std::string(void) const {
	std::string res = "";

	res += getBasicTypeName() + " " + name + " {\n\t";
	res += std::string(*formula) + "\n}";

	return res;
}

void bltlSymNode::acceptVisitor(symTabVisitor* visitor) {}

void bltlSymNode::acceptVisitor(symTabConstVisitor* visitor) const {}

/***************************************************************************************************************************************/

fMultiLTLSymNode::fMultiLTLSymNode(const std::string& name, const std::list<variantQuantifier*>& variants, expr* formula, int lineNb)
	: symbol(symbol::T_FMULTI_LTL, name, lineNb)
	, variants(variants)
	, formula(formula)
{
}

std::string fMultiLTLSymNode::getTypeName(void) const {
	return "fMultiLTL";
}

std::string fMultiLTLSymNode::getBasicTypeName(void) const {
	return "fMultiLTL";
}

int fMultiLTLSymNode::getTypeSize(void) const {
	return 0;
}

fMultiLTLSymNode::operator std::string(void) const {
	std::string res = "";

	res += getBasicTypeName() + " " + name + " ";
	int i = 0;
	for(auto it : variants) {
		res += std::string(*it) + (++i < ((int)variants.size()) ? ", " : "");
	}
	res += " {\n\t" + std::string(*formula) + "\n}";

	return res;
}

void fMultiLTLSymNode::acceptVisitor(symTabVisitor* visitor) {}

void fMultiLTLSymNode::acceptVisitor(symTabConstVisitor* visitor) const {}
