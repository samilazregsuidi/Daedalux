#include <daedalux/promela/ast/expr/runExpr.hpp>
#include <daedalux/promela/ast/expr/argExpr.hpp>
#include <daedalux/core/ast/visitor/astVisitor.hpp>
#include <daedalux/core/symbol/symTable.hpp>
#include <daedalux/promela/symbol/typedef/ptypeSymNode.hpp>

exprRun::exprRun(const std::string& procName, exprArgList *argList, int lineNb)
	: expr(astNode::E_EXPR_RUN, lineNb)
	, procName(procName)
{
	//assert(argList);
	addChild("arg_list", argList);
}

void exprRun::setArgList(exprArgList* argList) {
	eraseChild("arg_list", argList);
}

exprArgList* exprRun::getArgList(void) const {
	return dynamic_cast<exprArgList*>(getChild("arg_list"));
}

std::string exprRun::getProcName(void) const {
	return procName;
}

ptypeSymNode* exprRun::resolve(const symTable* symTab) {
	procSym = dynamic_cast<ptypeSymNode*>(symTab->lookup(procName));
	assert(procSym);
	return procSym;
}

const ptypeSymNode* exprRun::getProcType(void) const {
	return procSym;
}

exprRun::operator std::string() const {
	return "run " + procName + "(" + ( getArgList()? std::string(*getArgList()) : "" ) + ")";
}

std::string exprRun::getTypeDescr(void) const {
	return "Run (E_EXPR_RUN)";
}

symbol::Type exprRun::getExprType(void) const {
	return symbol::T_BYTE;
}

expr* exprRun::deepCopy(void) const {
	exprRun* copy = new exprRun(*this);
	copy->copyChildren(*this);
	return copy;
}

void exprRun::acceptVisitor(ASTConstVisitor* visitor) const {
	visitor->visit(this);
}

void exprRun::acceptVisitor(ASTVisitor* visitor) {
	visitor->visit(this);
}

