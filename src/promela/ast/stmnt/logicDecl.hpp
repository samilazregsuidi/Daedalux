#ifndef LOGIC_DECL_H
#define LOGIC_DECL_H

#include "stmnt.hpp"

class ltlSymNode;
//E_ltlDecl,				// symTab = declaration.
class ltlDecl : public stmnt
{
public:
	ltlDecl(ltlSymNode* sym, int lineNb);

	ltlDecl* getDeclSymTab(void) const;

	operator std::string() const override;

	std::string getTypeDescr(void) const override;

	unsigned int assignMutables(const Mask& mask = Mask(), unsigned int id = 0) override;

	bool mutateMutable(unsigned int id) override;

	stmnt* deepCopy(void) const override;

	void acceptVisitor(ASTConstVisitor* visitor) const override;

	void acceptVisitor(ASTVisitor* visitor) override;

private:
	ltlSymNode* sym;
};

class bltlSymNode;
//E_btlDecl,				// symTab = declaration.
class bltlDecl : public stmnt
{
public:
	bltlDecl(bltlSymNode* sym, int lineNb);

	bltlDecl* getDeclSymTab(void) const;

	operator std::string() const override;

	std::string getTypeDescr(void) const override;

	unsigned int assignMutables(const Mask& mask = Mask(), unsigned int id = 0) override;

	bool mutateMutable(unsigned int id) override;

	stmnt* deepCopy(void) const override;

	void acceptVisitor(ASTConstVisitor* visitor) const override;

	void acceptVisitor(ASTVisitor* visitor) override;

private:
	bltlSymNode* sym;
};


class fMultiLTLSymNode;

//E_fMultiLTLDecl,				// symTab = declaration.
class fMultiLTLDecl : public stmnt
{
public:
	fMultiLTLDecl(fMultiLTLSymNode* sym, int lineNb);

	fMultiLTLSymNode* getDeclSymTab(void) const;

	operator std::string() const override;

	std::string getTypeDescr(void) const override;

	unsigned int assignMutables(const Mask& mask = Mask(), unsigned int id = 0) override;

	bool mutateMutable(unsigned int id) override;

	stmnt* deepCopy(void) const override;

	void acceptVisitor(ASTConstVisitor* visitor) const override;

	void acceptVisitor(ASTVisitor* visitor) override;

private:
	fMultiLTLSymNode* sym;
};

#endif