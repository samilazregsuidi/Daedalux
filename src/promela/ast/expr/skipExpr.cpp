#include <assert.h>
#include <string>


#include <daedalux/promela/ast/expr/skipExpr.hpp>
#include <daedalux/core/symbol/symTable.hpp>
#include <daedalux/core/ast/visitor/astVisitor.hpp>


exprSkip::exprSkip(int lineNb)
	: expr(astNode::E_EXPR_SKIP, lineNb)
{
}

exprSkip::operator std::string() const {
	return "skip";
}

std::string exprSkip::getTypeDescr(void) const {
	return "Skip (E_EXPR_SKIP)";
}

symbol::Type exprSkip::getExprType(void) const {
	return symbol::T_BOOL;
}

expr* exprSkip::deepCopy(void) const {
	exprSkip* copy = new exprSkip(*this);
	return copy;
}

void exprSkip::acceptVisitor(ASTConstVisitor* visitor) const {
	visitor->visit(this);
}

void exprSkip::acceptVisitor(ASTVisitor* visitor) {
	visitor->visit(this);
}

/****************************************************************
 * **************************************************************
 * *************************************************************/
