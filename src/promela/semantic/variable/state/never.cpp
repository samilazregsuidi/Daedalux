#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "never.hpp"
#include "threadTransition.hpp"

#include "stateVisitor.hpp"

#include "payload.hpp"
#include "variable.hpp"

#include "automata.hpp"
#include "ast.hpp"

#include "initState.hpp"

#include "scalarVar.hpp"

never::never(const std::string& name, const fsmNode* start, ubyte pid)
	: thread(variable::V_NEVER, name, start, pid)
{
	//lines.push_back(edge->getLineNb());
}

never::never(const never& other) 
	: thread(other)
{
}

never* never::deepCopy(void) const {
	return new never(*this);
}

void never::init(void) {
	//assert(getProgState());

	variable::init();
	setFsmNodePointer(start);
}

std::list<transition*> never::transitions(void) const {
	auto node = getFsmNodePointer();
	std::list<transition*> res;
	for(auto e : node->getEdges())
		res.push_back(new threadTransition(const_cast<never*>(this), e));
	return res;
}

/**
 * Returns a list of all the executable transitions (for all the processes).
 * EFFECTS: None. (It CANNOT have any!)
 * WARNING:
 * 	In the end, does NOT (and must NEVER) modify the state payload.
 */
std::list<transition*> never::executables(void) const {

	std::list<transition*> res;

	const fsmNode* node = getFsmNodePointer();
	
	if(!node) 
		return res;

	for(auto edge : node->getEdges()) {

		if(eval(edge, EVAL_EXECUTABILITY) > 0) {

			res.push_back(new threadTransition(const_cast<never*>(this), edge));
		}
	}

	//warning else didnt work

	//assert(res.size() > 0);

	return res;
}

int never::eval(const fsmEdge* edge, byte flag) const {
	return eval(edge->getExpression(), flag);
}

int never::eval(const astNode* node, byte flag) const {

	assert(node);
	if(!node)
		return 0;

	auto binaryExpr = dynamic_cast<const exprBinary*>(node);
	auto unaryExpr = dynamic_cast<const exprUnary*>(node);

	switch(node->getType()) {

		case(astNode::E_VAR_DECL):
		case(astNode::E_PROC_DECL):
		case(astNode::E_CHAN_DECL):
		case(astNode::E_INIT_DECL):
		case(astNode::E_INLINE_DECL):
		case(astNode::E_TDEF_DECL):
		case(astNode::E_MTYPE_DECL):
			assert(false);

		case(astNode::E_STMNT_IF):
		case(astNode::E_STMNT_DO):
		case(astNode::E_STMNT_OPT):
		case(astNode::E_STMNT_SEQ):

		case(astNode::E_STMNT_BREAK):
		case(astNode::E_STMNT_GOTO):
		
		case(astNode::E_STMNT_LABEL):

		case(astNode::E_EXPR_TRUE):
		case(astNode::E_EXPR_SKIP):
		
			return 1;

		case(astNode::E_STMNT):
			assert(false);

		case(astNode::E_STMNT_EXPR):
			return eval(dynamic_cast<const stmntExpr*>(node)->getChild(), flag);

		case(astNode::E_EXPR_PAR):
			return eval(dynamic_cast<const exprPar*>(node)->getExpr(), flag);

		case(astNode::E_EXPR_VAR):
			return eval(dynamic_cast<const exprVar*>(node)->getVarRef(), flag);
		
		case(astNode::E_RARG_VAR):
			return eval(dynamic_cast<const exprRArgVar*>(node)->getVarRef(), flag);

		
		case(astNode::E_STMNT_ELSE):
			return (_else == 1);

		case(astNode::E_EXPR_RREF): 
		{	
			auto rref = dynamic_cast<const exprRemoteRef*>(node);
			auto isAtLabel = get<thread*>(getVarName(rref->getProcRef()))->isAtLabel(rref->getLabelLine());
			return isAtLabel;
		}

		case(astNode::E_EXPR_GT):
			return (eval(binaryExpr->getLeftExpr(), flag) > eval(binaryExpr->getRightExpr(), flag));

		case(astNode::E_EXPR_LT):
			return (eval(binaryExpr->getLeftExpr(), flag) < eval(binaryExpr->getRightExpr(), flag));

		case(astNode::E_EXPR_GE):
			return (eval(binaryExpr->getLeftExpr(), flag) >= eval(binaryExpr->getRightExpr(), flag));

		case(astNode::E_EXPR_LE):
			return (eval(binaryExpr->getLeftExpr(), flag) <= eval(binaryExpr->getRightExpr(), flag));

		case(astNode::E_EXPR_EQ): {
			//std::cout << std::string(*binaryExpr->getLeftExpr()) << "(" << eval(binaryExpr->getLeftExpr(), flag) << ") == " << std::string(*binaryExpr->getRightExpr()) << "(" << eval(binaryExpr->getRightExpr(), flag) << ")" << std::endl;
			//auto sanity = (eval(binaryExpr->getLeftExpr(), flag) == eval(binaryExpr->getRightExpr(), flag));
			return (eval(binaryExpr->getLeftExpr(), flag) == eval(binaryExpr->getRightExpr(), flag));
		}
		case(astNode::E_EXPR_NE):
			return (eval(binaryExpr->getLeftExpr(), flag) != eval(binaryExpr->getRightExpr(), flag));

		case(astNode::E_EXPR_AND):
			if(eval(binaryExpr->getLeftExpr(), flag)) 
				return eval(binaryExpr->getRightExpr(), flag);
			return 0;

		case(astNode::E_EXPR_OR):
			if(!eval(binaryExpr->getLeftExpr(), flag))
				return eval(binaryExpr->getRightExpr(), flag);
			return 1;

		case(astNode::E_EXPR_NEG):
			return !eval(unaryExpr->getExpr(), flag);

		case(astNode::E_EXPR_CONST):
			return dynamic_cast<const exprConst*>(node)->getCstValue();

		case(astNode::E_VARREF):
		{
			auto varRef = dynamic_cast<const exprVarRef*>(node);
			auto var = get<scalarInt*>(getVarName(varRef));
			if(!var) {
				var = get<scalarInt*>(getVarName(varRef));
				assert(false);
			}
			return var->getIntValue();
		}
		case(astNode::E_VARREF_NAME):
		{
			assert(false);
			auto varRefName = dynamic_cast<const exprVarRefName*>(node);
			return get<scalarInt*>(getVarName(varRefName))->getIntValue();
		}
		
		case(astNode::E_RARG_CONST):
			return dynamic_cast<const exprRArgConst*>(node)->getCstValue();

		case(astNode::E_EXPR_FALSE):
			return 0;

		case(astNode::E_ARGLIST):

		default:
			assert(false);
	}
	assert(false);
	return 0;
}


/**
 * Executes a statement and returns the new reached state. The transition must be executable.
 * The preserve parameter controls whether or not the state that is passed is preserved.
 *
 * The features expression of the processTransition is not modified. The value of this expression is
 * copied into the new state. Thus, when this state is destroyed, the features expression of the
 * processTransition is not deleted.
 *
 * assertViolation is a return value set to true in case the statement on the transition was an assert
 * that evaluated to false.
 */
void never::apply(transition* trans) {
	auto threadTrans = dynamic_cast<const threadTransition*>(trans);
	assert(threadTrans);
	auto proc = dynamic_cast<const never*>(threadTrans->getThread());
	auto edge = threadTrans->getEdge();

	assert(proc);
	assert(edge);

	//assert(origin == nullptr);

	auto expression = edge->getExpression();

	//_assertViolation = 0;

Apply:

	switch(expression->getType()) {
		case(astNode::E_VAR_DECL):
		case(astNode::E_STMNT):
			assert(false);
			break;

		case(astNode::E_STMNT_IF):
		case(astNode::E_STMNT_DO):
		case(astNode::E_STMNT_OPT):
		case(astNode::E_STMNT_SEQ):
		case(astNode::E_STMNT_LABEL):
			//failure("Found control statement while applying an expression at line %2d\n", expression->lineNb);
			assert(false);
			break;

		case(astNode::E_STMNT_BREAK):
		case(astNode::E_STMNT_GOTO):
			break;

		case(astNode::E_STMNT_ELSE):
			break;

		case(astNode::E_STMNT_EXPR):
		{
			expression = dynamic_cast<const stmntExpr*>(expression)->getChild();
			goto Apply;
		}
		case(astNode::E_EXPR_PAR):
		{
			expression = dynamic_cast<const exprPar*>(expression)->getExpr();
			goto Apply;
		}

		case(astNode::E_EXPR_PLUS):
		case(astNode::E_EXPR_MINUS):
		case(astNode::E_EXPR_TIMES):
		case(astNode::E_EXPR_DIV):
		case(astNode::E_EXPR_MOD):
		case(astNode::E_EXPR_UMIN):
			//failure("Found arithmetic expression while applying an expression at line %2d\n", expression->lineNb);
			//apply should modify a lvalue variable
			assert(false);
			break;

		case(astNode::E_RARG_EVAL):
		case(astNode::E_EXPR_CONST):
		case(astNode::E_EXPR_GT):
		case(astNode::E_EXPR_LT):
		case(astNode::E_EXPR_GE):
		case(astNode::E_EXPR_LE):
		case(astNode::E_EXPR_EQ):
		case(astNode::E_EXPR_NE):
		case(astNode::E_EXPR_AND):
		case(astNode::E_EXPR_OR):
		case(astNode::E_EXPR_NEG):
		case(astNode::E_EXPR_LSHIFT):
		case(astNode::E_EXPR_RSHIFT):
		case(astNode::E_EXPR_BITWAND):
		case(astNode::E_EXPR_BITWOR):
		case(astNode::E_EXPR_BITWXOR):
		case(astNode::E_EXPR_BITWNEG):
		case(astNode::E_EXPR_COND):
		case(astNode::E_EXPR_TIMEOUT):
		case(astNode::E_EXPR_FULL):
		case(astNode::E_EXPR_NFULL):
		case(astNode::E_EXPR_EMPTY):
		case(astNode::E_EXPR_NEMPTY):
		case(astNode::E_EXPR_RREF):
			break;

		case(astNode::E_EXPR_TRUE):
		case(astNode::E_EXPR_SKIP):
			break;

		case(astNode::E_EXPR_LEN):
		case(astNode::E_EXPR_VAR):
		case(astNode::E_ARGLIST):
		case(astNode::E_VARREF):
		case(astNode::E_VARREF_NAME):
		case(astNode::E_RARG_VAR):
		case(astNode::E_RARG_CONST):
			break;
		
		default:
			assert(false);
			break;
	}

	// Proceed in automaton
	setFsmNodePointer(edge->getTargetNode());

	origin = trans;
	assert(trans->dst == nullptr);
	trans->dst = this;

	//std::cout << this->getFullName() << "::apply (" << oldLocation << ", " << dynamic_cast<const neverTransition*>(trans)->getEdge()->getLineNb() << ", " << getLocation() << ")" << std::endl;
}


void never::print(void) const {
	auto node = getFsmNodePointer();
	if(node)	printf("never                               @ NL%02u %s\n", node->getLineNb(), node->getFlags() & fsmNode::N_ACCEPT ? " (accepting)" : "");
	else 		printf("never                               @ end\n");
	
	variable::print();
}

void never::printCSVHeader(std::ostream &out) const {
	out << "never,";
}

void never::printCSV(std::ostream &out) const {
	out << getLocation() << ",";
}

bool never::isAccepting(void) const {
	return getFsmNodePointer()->isAccepting();
}

bool never::safetyPropertyViolation(void) const {
	return endstate();
}

state* never::getNeverClaim(void) const {
	return const_cast<never*>(this);
}

void never::accept(stateVisitor* visitor) {
	visitor->visit(this);
}