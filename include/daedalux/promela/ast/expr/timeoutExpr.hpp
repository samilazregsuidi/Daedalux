#ifndef TIMEOUT_EXPR_H
#define TIMEOUT_EXPR_H

#include <daedalux/core/ast/expr/exprNode.hpp>


//E_EXPR_TIMEOUT,	// iVal = 0
class exprTimeout : public expr
{
public:
	exprTimeout(int lineNb);

	operator std::string() const override;

	std::string getTypeDescr(void) const override;

	symbol::Type getExprType(void) const override;

	expr* deepCopy(void) const override;

	void acceptVisitor(ASTConstVisitor* visitor) const override;

	void acceptVisitor(ASTVisitor* visitor) override;
};

#endif
