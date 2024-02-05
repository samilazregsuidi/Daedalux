#include <assert.h>
#include <string>

#include "logicDecl.hpp"
#include "temporalSymNode.hpp"
#include "constExpr.hpp"

#include "symbol.hpp"
#include "varSymNode.hpp"

#include "astVisitor.hpp"

#include "utypeSymNode.hpp"
#include "tdefSymNode.hpp"
#include "ptypeSymNode.hpp"
#include "inlineSymNode.hpp"

ltlDecl::ltlDecl(ltlSymNode* sym, int lineNb)
	: stmnt(astNode::E_LTL_DECL, lineNb)
	, sym(sym)
{}

std::string ltlDecl::getTypeDescr(void) const {
	return "LTL declaration wrapper (E_LTL_DECL)";
}

stmnt* ltlDecl::deepCopy(void) const {
	ltlDecl* copy = new ltlDecl(*this);
	copy->prev = copy;
	copy->copyChildren(*this);
	//if(copy->getNext())
	//	return stmnt::merge(copy, getNext()->deepCopy());
	return copy;
}

unsigned int ltlDecl::assignMutables(const Mask& mask, unsigned int id) {
	assert(false);
	return id;
}

bool ltlDecl::mutateMutable(unsigned int id){
	assert(false);
	return false;
}

ltlDecl::operator std::string() const {
	return std::string(*sym);
}

void ltlDecl::acceptVisitor(ASTConstVisitor* visitor) const {}

void ltlDecl::acceptVisitor(ASTVisitor* visitor) {}

/****************************************************************************/

bltlDecl::bltlDecl(bltlSymNode* sym, int lineNb)
	: stmnt(astNode::E_BLTL_DECL, lineNb)
	, sym(sym)
{}

std::string bltlDecl::getTypeDescr(void) const {
	return "BLTL declaration wrapper (E_BLTL_DECL)";
}

stmnt* bltlDecl::deepCopy(void) const {
	bltlDecl* copy = new bltlDecl(*this);
	copy->prev = copy;
	copy->copyChildren(*this);
	//if(copy->getNext())
	//	return stmnt::merge(copy, getNext()->deepCopy());
	return copy;
}

unsigned int bltlDecl::assignMutables(const Mask& mask, unsigned int id) {
	assert(false);
	return id;
}

bool bltlDecl::mutateMutable(unsigned int id){
	assert(false);
	return false;
}

bltlDecl::operator std::string() const {
	return std::string(*sym);
}

void bltlDecl::acceptVisitor(ASTConstVisitor* visitor) const {}

void bltlDecl::acceptVisitor(ASTVisitor* visitor) {}

/*********************************************************************/

fMultiLTLDecl::fMultiLTLDecl(fMultiLTLSymNode* sym, int lineNb)
	: stmnt(astNode::E_FMULTI_LTL_DECL, lineNb)
	, sym(sym)
{}

std::string fMultiLTLDecl::getTypeDescr(void) const {
	return "fMultiLTL declaration wrapper (E_FMULTI_LTL_DECL)";
}

stmnt* fMultiLTLDecl::deepCopy(void) const {
	fMultiLTLDecl* copy = new fMultiLTLDecl(*this);
	copy->prev = copy;
	copy->copyChildren(*this);
	//if(copy->getNext())
	//	return stmnt::merge(copy, getNext()->deepCopy());
	return copy;
}

unsigned int fMultiLTLDecl::assignMutables(const Mask& mask, unsigned int id) {
	assert(false);
	return id;
}

bool fMultiLTLDecl::mutateMutable(unsigned int id){
	assert(false);
	return false;
}

fMultiLTLDecl::operator std::string() const {
	return std::string(*sym);
}

void fMultiLTLDecl::acceptVisitor(ASTConstVisitor* visitor) const {}

void fMultiLTLDecl::acceptVisitor(ASTVisitor* visitor) {}
