#include <assert.h>
#include <string>

#include <daedalux/core/ast/visitor/astVisitor.hpp>
#include <daedalux/promela/ast/expr/timeoutExpr.hpp>
#include <daedalux/core/symbol/symTable.hpp>


/****************************************************************
 * **************************************************************
 * *************************************************************/

exprTimeout::exprTimeout(int lineNb)
	: expr(astNode::E_EXPR_TIMEOUT, lineNb)
{
}

exprTimeout::operator std::string() const {
	return "timeout";
}

std::string exprTimeout::getTypeDescr(void) const {
	return "Timeout (E_EXPR_TIMEOUT)";
}

symbol::Type exprTimeout::getExprType(void) const {
	return symbol::T_BOOL;
}

expr* exprTimeout::deepCopy(void) const {
	exprTimeout* copy = new exprTimeout(*this);
	return copy;
}

void exprTimeout::acceptVisitor(ASTConstVisitor* visitor) const {
	visitor->visit(this);
}

void exprTimeout::acceptVisitor(ASTVisitor* visitor) {
	visitor->visit(this);
}

