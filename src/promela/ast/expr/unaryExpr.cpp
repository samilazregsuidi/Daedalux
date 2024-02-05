#include "unaryExpr.hpp"

#include "astVisitor.hpp"

/****************************************************************
 * **************************************************************
 * *************************************************************/

exprUnary::exprUnary(Type type, expr* mExpr, int lineNb)
    : expr(type, lineNb)
{
    assert(mExpr);
    addChild("expr", mExpr);
}

void exprUnary::setExpr(expr* mExpr) {
    eraseChild("expr", mExpr);
}

expr* exprUnary::getExpr(void) const {
    return dynamic_cast<expr*>(getChild("expr"));
}

symbol::Type exprUnary::getExprType(void) const {
    return getExpr()->getExprType();
}

void exprPar::acceptVisitor(ASTConstVisitor* visitor) const {
	visitor->visit(this);
}

void exprPar::acceptVisitor(ASTVisitor* visitor) {
	visitor->visit(this);
}

void exprCount::acceptVisitor(ASTConstVisitor* visitor) const {
	visitor->visit(this);
}

void exprCount::acceptVisitor(ASTVisitor* visitor) {
	visitor->visit(this);
}

void exprUMin::acceptVisitor(ASTConstVisitor* visitor) const {
	visitor->visit(this);
}

void exprUMin::acceptVisitor(ASTVisitor* visitor) {
	visitor->visit(this);
}

void exprNeg::acceptVisitor(ASTConstVisitor* visitor) const {
	visitor->visit(this);
}

void exprNeg::acceptVisitor(ASTVisitor* visitor) {
	visitor->visit(this);
}

void exprBitwNeg::acceptVisitor(ASTConstVisitor* visitor) const {
	visitor->visit(this);
}

void exprBitwNeg::acceptVisitor(ASTVisitor* visitor) {
	visitor->visit(this);
}

void exprLen::acceptVisitor(ASTConstVisitor* visitor) const {
	visitor->visit(this);
}

void exprLen::acceptVisitor(ASTVisitor* visitor) {
	visitor->visit(this);
}

void exprFull::acceptVisitor(ASTConstVisitor* visitor) const {
	visitor->visit(this);
}

void exprFull::acceptVisitor(ASTVisitor* visitor) {
	visitor->visit(this);
}

void exprNFull::acceptVisitor(ASTConstVisitor* visitor) const {
	visitor->visit(this);
}

void exprNFull::acceptVisitor(ASTVisitor* visitor) {
	visitor->visit(this);
}

void exprEmpty::acceptVisitor(ASTConstVisitor* visitor) const {
	visitor->visit(this);
}

void exprEmpty::acceptVisitor(ASTVisitor* visitor) {
	visitor->visit(this);
}

void exprNEmpty::acceptVisitor(ASTConstVisitor* visitor) const {
	visitor->visit(this);
}

void exprNEmpty::acceptVisitor(ASTVisitor* visitor) {
	visitor->visit(this);
}

/****************************************************************
 * **************************************************************
 * *************************************************************/

/****************************************************************
 * **************************************************************
 * *************************************************************/

unsigned int exprNeg::assignMutables(const Mask& mask, unsigned int id) {
	if(mask.isPresent(type)) {
		mId = ++id;
		for(auto c : children)
			id = c.second->assignMutables(mask, id);
	}
	return id;
}

std::vector<astNode*> exprNeg::getMutations(void) const {
    return { new exprPar(dynamic_cast<expr*>(getExpr()->deepCopy()), lineNb) };
}