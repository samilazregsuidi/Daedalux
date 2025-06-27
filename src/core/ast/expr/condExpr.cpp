#include <assert.h>
#include <string>

#include <daedalux/core/ast/visitor/astVisitor.hpp>
#include <daedalux/core/ast/expr/condExpr.hpp>
#include <daedalux/core/symbol/symTable.hpp>

exprCond::exprCond(expr *pcond, expr *pthen, expr *pelsE, int lineNb)
	: expr(astNode::E_EXPR_COND, lineNb)
{
	assert(pcond);
	assert(pthen);
	assert(pelsE);
	addChild("cond_expr", pcond);
	addChild("then_expr", pthen);
	addChild("else_expr", pelsE);
}

void exprCond::setCond(expr* cond) {
	eraseChild("cond_expr", cond);
}

void exprCond::setThen(expr* then) {
	eraseChild("then_expr", then);
}

void exprCond::setElse(expr* elsE) {
	eraseChild("else_expr", elsE);
}

expr* exprCond::getCond(void) const {
	return dynamic_cast<expr*>(getChild("cond_expr"));
}

expr* exprCond::getThen(void) const {
	return dynamic_cast<expr*>(getChild("then_expr"));
}

expr* exprCond::getElse(void) const {
	return dynamic_cast<expr*>(getChild("else_expr"));
}

exprCond::operator std::string() const {
	assert(getCond() && getThen() && getElse());
	return "(" + std::string(*getCond()) + "-> " + std::string(*getThen()) + ": " + std::string(*getElse())+ ")";
}

std::string exprCond::getTypeDescr(void) const {
	return "Conditional expression (E_EXPR_COND)";
}

symbol::Type exprCond::getExprType(void) const {
	return expr::getExprType(getThen(), getElse());
}

expr* exprCond::deepCopy(void) const {
	exprCond* copy = new exprCond(*this);
	copy->copyChildren(*this);
	return copy;
}

void exprCond::acceptVisitor(ASTConstVisitor* visitor) const {
	visitor->visit(this);
}

void exprCond::acceptVisitor(ASTVisitor* visitor) {
	visitor->visit(this);
}
