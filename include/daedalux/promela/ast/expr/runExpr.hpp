#ifndef RUN_EXPR_H
#define RUN_EXPR_H

#include <daedalux/core/ast/expr/exprNode.hpp>

class symTable;
class exprArgList;
class ptypeSymNode;

//E_EXPR_RUN,			// child[0] = E_ARGLIST, sVal = the procType name, and after processing: symTab = node in symbol table that represents the proctype
class exprRun : public expr
{
public:
	exprRun(const std::string& procName, exprArgList *argList, int lineNb);

	std::string getProcName(void) const;

	const ptypeSymNode* getProcType(void) const;

	ptypeSymNode* resolve(const symTable* symTab);

	void setArgList(exprArgList* argList);

	exprArgList* getArgList(void) const;

	operator std::string() const override;

	std::string getTypeDescr(void) const override;

	symbol::Type getExprType(void) const override;

	expr* deepCopy(void) const override;

	void acceptVisitor(ASTConstVisitor* visitor) const override;

	void acceptVisitor(ASTVisitor* visitor) override;

private:
	std::string procName;
	ptypeSymNode* procSym;
};

#endif
