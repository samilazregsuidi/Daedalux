#include <assert.h>
#include <string>

#include <daedalux/core/ast/visitor/astVisitor.hpp>
#include <daedalux/core/ast/expr/exprNode.hpp>
#include <daedalux/core/symbol/symTable.hpp>

/****************************************************************
 * **************************************************************
 * *************************************************************/

expr::expr(Type type, int lineNb)
	: astNode(type, lineNb)
{
	exprType = symbol::T_NA;
}

symbol::Type expr::getExprType(void) const {
	return exprType;
}

void expr::setExprType(symbol::Type exprType) {
	this->exprType = exprType;
}

bool expr::castToExprType(symbol::Type type) const {
	type = type;//keep compiler happy
	return false;
}

symbol::Type expr::getExprType(expr* left, expr* right) {
	if(left->getExprType() == right->getExprType())
		return left->getExprType();
	else if(left->castToExprType(right->getExprType()))
		return right->getExprType();
	else if(right->castToExprType(left->getExprType()))
		return left->getExprType();

	return symbol::T_NA;
}

/*void expr::acceptVisitor(ASTConstVisitor* visitor) const {
	visitor->visit(this);
}

void expr::acceptVisitor(ASTVisitor* visitor) {
	visitor->visit(this);
}*/