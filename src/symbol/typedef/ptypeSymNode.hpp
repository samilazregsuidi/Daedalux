#ifndef PTYPE_SYM_NODE_H
#define PTYPE_SYM_NODE_H

#include <list>

#include "symbol.hpp"

#include "varSymNode.hpp"

class seqSymNode : public symbol, public complexSymNode {
protected:
	seqSymNode(Type type, const std::string& name, int lineNb, stmnt* block);
	
public:
	stmnt* getBlock(void) const;

	operator std::string(void) const override;

	symTable* getSymTable(void) const;

	symTable* getSubSymTable(void) const override;

protected:
	stmnt* block;
};

class initSymNode : public seqSymNode {
public:
	initSymNode(int lineNb, stmnt* block);

	std::string getTypeName(void) const override;

	std::string getBasicTypeName(void) const override;

	int getTypeSize(void) const override;	

	void acceptVisitor(symTabVisitor* visitor) override;

	void acceptVisitor(symTabConstVisitor* visitor) const override;

	//void printGraphViz(std::ofstream& file) const;

};

class neverSymNode : public seqSymNode {
public:
	neverSymNode(int lineNb, stmnt* block);

	std::string getTypeName(void) const override;

	std::string getBasicTypeName(void) const override;

	int getTypeSize(void) const override;

	void acceptVisitor(symTabVisitor* visitor) override;

	void acceptVisitor(symTabConstVisitor* visitor) const override;

	//void printGraphViz(std::ofstream& file) const;
};

class exprConst;

//T_PROC
class ptypeSymNode : public seqSymNode {
public:
	ptypeSymNode(const std::string& name, exprConst* active, const std::list<varSymNode*>& args, stmnt* block, int lineNb);

	~ptypeSymNode();

	std::string getTypeName(void) const override;

	std::string getBasicTypeName(void) const override;

	int getTypeSize(void) const override;

	//void resolveVariables(symbol* globalSymTab, const mTypeList* mTypes, varSymNode* localSymTab = nullptr, symbol* subFieldSymTab = nullptr) override ;
	exprConst* getActiveExpr(void) const;

	const std::list<varSymNode*>& getArgs(void) const;

	//unsigned int processVariables(symbol* global, const mTypeList* mTypes, unsigned int offset, bool isGlobal) override ;

	operator std::string(void) const override;

	void acceptVisitor(symTabVisitor* visitor) override;

	void acceptVisitor(symTabConstVisitor* visitor) const override;

	//void printGraphViz(std::ofstream& file) const;

private:
	std::list<varSymNode*> args;
	exprConst* active;
};

#endif
