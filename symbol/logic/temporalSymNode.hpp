#ifndef TEMPORAL_SYM_NODE
#define TEMPORAL_SYM_NODE

#include <list>

#include "symbol.hpp"

//T_LTL
class ltlSymNode : public symbol {
public:
	ltlSymNode(const std::string& name, expr* formula, int lineNb);

	std::string getTypeName(void) const override;

	std::string getBasicTypeName(void) const override;

	int getTypeSize(void) const override;

	const std::list<std::string>& getParams(void) const;

	operator std::string(void) const override;

	void acceptVisitor(symTabVisitor* visitor) override;

	void acceptVisitor(symTabConstVisitor* visitor) const override;

	//void printGraphViz(std::ofstream& file) const;

private:
	expr* formula;
};

//T_BLTL
class bltlSymNode : public symbol {
public:
	bltlSymNode(const std::string& name, expr* formula, int lineNb);

	std::string getTypeName(void) const override;

	std::string getBasicTypeName(void) const override;

	int getTypeSize(void) const override;

	const std::list<std::string>& getParams(void) const;

	operator std::string(void) const override;

	void acceptVisitor(symTabVisitor* visitor) override;

	void acceptVisitor(symTabConstVisitor* visitor) const override;

	//void printGraphViz(std::ofstream& file) const;

private:
	expr* formula;
};

class variantQuantifier;

//T_FMULTI_LTL
class fMultiLTLSymNode : public symbol {
public:
	fMultiLTLSymNode(const std::string& name, const std::list<variantQuantifier*>& variants, expr* formula, int lineNb);

	std::string getTypeName(void) const override;

	std::string getBasicTypeName(void) const override;

	int getTypeSize(void) const override;

	const std::list<std::string>& getParams(void) const;

	operator std::string(void) const override;

	void acceptVisitor(symTabVisitor* visitor) override;

	void acceptVisitor(symTabConstVisitor* visitor) const override;

	//void printGraphViz(std::ofstream& file) const;

private:
	std::list<variantQuantifier*> variants;
	expr* formula;
};

#endif
