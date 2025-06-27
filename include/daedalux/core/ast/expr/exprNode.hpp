#ifndef EXPR_H
#define EXPR_H

#include <vector>
#include <memory>

#include "../astNode.hpp"

#include "../../symbol/symNode.hpp"

class expr : public astNode
{
protected:
	expr(Type type, int lineNb);

public:
	virtual symbol::Type getExprType(void) const;

	void setExprType(symbol::Type exprType);

	virtual bool castToExprType(symbol::Type type) const;

	static symbol::Type getExprType(expr* left, expr* right);
	
protected:
	symbol::Type exprType;
};



#endif
