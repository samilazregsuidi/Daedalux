#ifndef SYS_SYM_NODE_H
#define SYS_SYM_NODE_H

#include "varSymNode.hpp"

//T_SYS
class sysSymNode : public varSymNode, public complexSymNode {
public:
	sysSymNode(int lineNb, const std::string& name, unsigned int bound = 1, expr* init = nullptr);

	std::string getBasicTypeName(void) const override;

	std::string getTypeName(void) const override;

	symTable* getSubSymTable(void) const override;

	int getTypeSize(void) const override;

	operator std::string(void) const override;

	//void printGraphViz(std::ofstream& file) const;

	void acceptVisitor(symTabVisitor* visitor) override;

	void acceptVisitor(symTabConstVisitor* visitor) const override;
};

#endif