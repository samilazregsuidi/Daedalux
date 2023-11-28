#ifndef EXP_TO_ADD_H
#define EXP_TO_ADD_H

#include "astVisitor.hpp"
#include "tvl.hpp"

class expToADD : public ASTConstVisitor{
public:
	expToADD(const TVL* fm);
	~expToADD() override;
	//void visit(const expr* node) override;
	void visit(const exprVarRefName* node) override;
	void visit(const exprVarRef* node) override;
	void visit(const exprVar* node) override;
	void visit(const exprConst* node) override;
	void visit(const exprTrue* node) override;
	void visit(const exprFalse* node) override;
	void visit(const exprPlus* node) override;
	void visit(const exprMinus* node) override;
	void visit(const exprTimes* node) override;
	void visit(const exprAnd* node) override;
	void visit(const exprOr* node) override;
	void visit(const exprPar* node) override;
	void visit(const exprUMin* node) override;
	void visit(const exprNeg* node) override;

	ADD getFormula(void) const;
	bool isFeatureExpr(void) const;

private:
	const TVL* fm;
	Cudd* mgr;
	ADD current;
	bool featureRef;
};

#endif
