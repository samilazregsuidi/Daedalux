#include "expToADD.hpp"

#include "ast.hpp"

#include <assert.h>

expToADD::expToADD(const TVL* fm)
	: fm(fm)
	, mgr(fm->getMgr())
	, featureRef(false)
{
	// assert(mgr);
}

expToADD::~expToADD() {

}

/*void expToADD::visit(const expr* node)  {
	assert(false); node = node;
}*/

void expToADD::visit(const exprVarRefName* node) {
	auto varSym = dynamic_cast<varSymNode*>(node->getSymbol());
	if(varSym->getInitExpr())
		varSym->getInitExpr()->acceptVisitor(this);
}

void expToADD::visit(const exprVarRef* node) {
	if(node->getVarRefName()->getSymbol()->getType() == symbol::T_UTYPE){
		auto uTypeFeature = dynamic_cast<utypeSymNode*>(node->getVarRefName()->getSymbol());
		assert(uTypeFeature);
		if (uTypeFeature->getUType()->getName() == "features") {
			assert(node->hasSubField() && !node->getSubField()->hasSubField());
			auto featureName = node->getSubField()->getVarRefName()->getName();
			assert(fm->hasFeature(featureName));
			current = fm->getFeature(featureName).Add();
			featureRef = true;
		} else {
			node->getSubField()->acceptVisitor(this);
		}
	} else {
		//assert(!node->hasSubField());
		node->getVarRefName()->acceptVisitor(this);
	}
}

void expToADD::visit(const exprVar* node) {
	node->getVarRef()->acceptVisitor(this);
}

void expToADD::visit(const exprConst* node)  {
	current = mgr->constant(node->getCstValue());
}

void expToADD::visit(const exprTrue* node)  {
	current = mgr->addOne();
}

void expToADD::visit(const exprFalse* node)  {
	current = mgr->addZero();
}

void expToADD::visit(const exprPlus* node)  {
	node->getLeftExpr()->acceptVisitor(this);
	auto leftFormula = current;

	node->getRightExpr()->acceptVisitor(this);
	auto rightFormula = current;

	if(leftFormula && rightFormula)
		current = leftFormula + rightFormula;
}

void expToADD::visit(const exprMinus* node)  {
	node->getLeftExpr()->acceptVisitor(this);
	auto leftFormula = current;

	node->getRightExpr()->acceptVisitor(this);
	auto rightFormula = current;

	if(leftFormula && rightFormula)
		current = leftFormula - rightFormula;
}

void expToADD::visit(const exprTimes* node)  {
	node->getLeftExpr()->acceptVisitor(this);
	auto leftFormula = current;

	node->getRightExpr()->acceptVisitor(this);
	auto rightFormula = current;

	if(leftFormula && rightFormula)
		current = leftFormula + rightFormula;
}

void expToADD::visit(const exprAnd* node)  {
	node->getLeftExpr()->acceptVisitor(this);
	auto leftFormula = current;

	node->getRightExpr()->acceptVisitor(this);
	auto rightFormula = current;

	if(leftFormula && rightFormula)
		current = leftFormula & rightFormula;
}

void expToADD::visit(const exprOr* node)  {
	node->getLeftExpr()->acceptVisitor(this);
	auto leftFormula = current;

	node->getRightExpr()->acceptVisitor(this);
	auto rightFormula = current;

	if(leftFormula && rightFormula)
		current = leftFormula | rightFormula;
}

void expToADD::visit(const exprPar* node)  {
	node->getExpr()->acceptVisitor(this);
}

void expToADD::visit(const exprUMin* node)  {
	node->getExpr()->acceptVisitor(this);

	if(current)
		current = -current;
}

void expToADD::visit(const exprNeg* node)  {
	node->getExpr()->acceptVisitor(this);

	if(current)
		current = ~current;
}

ADD expToADD::getFormula(void) const {
	return current;
}

bool expToADD::isFeatureExpr(void) const {
	return featureRef;
}
