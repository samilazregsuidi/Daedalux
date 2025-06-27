#ifndef COND_EXPR_H
#define COND_EXPR_H

#include "exprNode.hpp"

//E_EXPR_COND,		// child[0] = E_EXPR_* (the condition), child[1] = E_EXPR_* (then), child[2] = E_EXPR_* (else)
class exprCond : public expr
{
public:
	exprCond(expr *pcond, expr *pthen, expr *pelsE, int lineNb);

	void setCond(expr* cond);

	void setThen(expr* then);

	void setElse(expr* elsE);

	expr* getCond(void) const;

	expr* getThen(void) const;

	expr* getElse(void) const;

	operator std::string() const override;

	std::string getTypeDescr(void) const override;

	symbol::Type getExprType(void) const override;

	expr* deepCopy(void) const override;

	void acceptVisitor(ASTConstVisitor* visitor) const override;

	void acceptVisitor(ASTVisitor* visitor) override;
};




#endif
