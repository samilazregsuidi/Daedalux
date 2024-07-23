#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "stateVisitor.hpp"

#include "process.hpp"
#include "transition.hpp"
#include "threadTransition.hpp"
#include "rendezVousTransition.hpp"
#include "progTransition.hpp"
#include "program.hpp"

#include "payload.hpp"
#include "variable.hpp"

#include "automata.hpp"
#include "ast.hpp"

#include "initState.hpp"

#include "channelVar.hpp"

process::process(const std::string& name, const fsmNode* start)
	: thread(variable::V_PROC, name, start, 0)
{}

process::process(const process& other) 
	: thread(other)
{
}

process::~process() {

}

process* process::deepCopy(void) const {
	auto copy = new process(*this);
	return copy;
}

void process::setProgState(program* newS) {
	setParent(newS);
}

program* process::getProgState(void) const {
	return dynamic_cast<program*>(getParent());
}

bool process::isAccepting(void) const {
	return false;
}

bool process::safetyPropertyViolation(void) const {
	return false;
}

state* process::getNeverClaim(void) const {
	return parent? dynamic_cast<state*>(parent)->getNeverClaim() : nullptr;
}

channel* process::getChannel(const std::string& name) const {

  auto var = get(name);

  if (!var)
    return nullptr;

  channel * chan = nullptr;
  if (var->getType() == variable::V_CID) {
	auto casted = dynamic_cast<CIDVar*>(var);
	assert(casted);
    chan = get<channel*>(casted->getRefChannel());
    assert(chan);
  }
  else {
    assert(var->getType() == variable::V_CHAN);
    chan = dynamic_cast<channel *>(var);
  }
  assert(chan);
  return chan;
}

std::list<transition*> process::transitions(void) const {
	auto node = getFsmNodePointer();
	std::list<transition*> res;
	for(auto e : node->getEdges())
		res.push_back(new threadTransition(const_cast<process*>(this), e));
	return res;
}


int process::eval(const fsmEdge* edge, byte flag) const {
	return eval(edge->getExpression(), flag);
}

int process::eval(const astNode* node, byte flag) const {

	assert(node);
	if(!node)	
		return 0;

	program* s = getProgState();

	//MODE : HANDSHAKE REQUEST TO MEET
	if(flag == EVAL_EXECUTABILITY && s->hasHandShakeRequest() && node->getType() != astNode::E_STMNT_CHAN_RCV)
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

		case(astNode::E_STMNT_ACTION):

		case(astNode::E_STMNT_BREAK):
		case(astNode::E_STMNT_GOTO):
		
		case(astNode::E_STMNT_LABEL):
		case(astNode::E_STMNT_ASGN):
		case(astNode::E_STMNT_PRINT):
		case(astNode::E_STMNT_PRINTM):
		case(astNode::E_STMNT_ASSERT):

		case(astNode::E_EXPR_RUN):
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

		case(astNode::E_RARG_EVAL):
			return eval(dynamic_cast<const exprRArgEval*>(node)->getToEval(), flag);

		case(astNode::E_STMNT_CHAN_SND):
		{
			channel* chan = get<channel*>(getVarName(dynamic_cast<const stmntChanSnd*>(node)->getChan()));

			if (chan->isRendezVous()) {
				// We check if the rendezvous can be completed.
				return s->requestHandShake({chan, this});

			} else
				// Ok if there is space in the channel
				return !chan->full();
			
		}

		case(astNode::E_STMNT_CHAN_RCV):
		{
			auto chanRecvStmnt = dynamic_cast<const stmntChanRecv*>(node);
			assert(chanRecvStmnt);
			channel* chan = getChannel(getVarName((chanRecvStmnt->getChan())));
			assert(chan);

			// Handshake request does not concern the channel or no message in the channel
			if ((chan->isRendezVous() && chan != s->getHandShakeRequestChan()) || (!chan->isRendezVous() && chan->empty())) {
				return 0;

			} else {
				// Either a rendezvous concerns the channel, either the channel has a non null capacity and is not empty.
				
				return chan->isReceivable(getOutputParamList(chanRecvStmnt->getRArgList()));
			}
		}

		case(astNode::E_STMNT_INCR):
		{
			if(flag == EVAL_EXECUTABILITY) 
				return 1;
			else 
				return eval(dynamic_cast<const stmntIncr*>(node)->getVarRef(), flag) + 1;
		}
		case(astNode::E_STMNT_DECR):
		{
			if(flag == EVAL_EXECUTABILITY) 
				return 1;
			else 
				return eval(dynamic_cast<const stmntDecr*>(node)->getVarRef(), flag) - 1;
		}
		case(astNode::E_STMNT_ELSE):
			return (_else == 1);

		case(astNode::E_EXPR_RREF): 
		{	
			auto rref = dynamic_cast<const exprRemoteRef*>(node);
			return get<process*>(getVarName((rref->getProcRef())))->isAtLabel(rref->getLabelLine());
		}

		case(astNode::E_EXPR_PLUS):
			return eval(binaryExpr->getLeftExpr(), flag) + eval(binaryExpr->getRightExpr(), flag);

		case(astNode::E_EXPR_MINUS):
			return eval(binaryExpr->getLeftExpr(), flag) - eval(binaryExpr->getRightExpr(), flag);

		case(astNode::E_EXPR_TIMES):
			return eval(binaryExpr->getLeftExpr(), flag) * eval(binaryExpr->getRightExpr(), flag);

		case(astNode::E_EXPR_DIV):
			return eval(binaryExpr->getLeftExpr(), flag) / eval(binaryExpr->getRightExpr(), flag);

		case(astNode::E_EXPR_MOD):
			return eval(binaryExpr->getLeftExpr(), flag) % eval(binaryExpr->getRightExpr(), flag);

		case(astNode::E_EXPR_GT):
			return (eval(binaryExpr->getLeftExpr(), flag) > eval(binaryExpr->getRightExpr(), flag));

		case(astNode::E_EXPR_LT):
			return (eval(binaryExpr->getLeftExpr(), flag) < eval(binaryExpr->getRightExpr(), flag));

		case(astNode::E_EXPR_GE):
			return (eval(binaryExpr->getLeftExpr(), flag) >= eval(binaryExpr->getRightExpr(), flag));

		case(astNode::E_EXPR_LE):
			return (eval(binaryExpr->getLeftExpr(), flag) <= eval(binaryExpr->getRightExpr(), flag));

		case(astNode::E_EXPR_EQ):
			return (eval(binaryExpr->getLeftExpr(), flag) == eval(binaryExpr->getRightExpr(), flag));

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

		case(astNode::E_EXPR_UMIN):
			return - eval(unaryExpr->getExpr(), flag);

		case(astNode::E_EXPR_NEG):
			return !eval(unaryExpr->getExpr(), flag);

		case(astNode::E_EXPR_LSHIFT):
			return eval(binaryExpr->getLeftExpr(), flag) << eval(binaryExpr->getRightExpr(), flag);

		case(astNode::E_EXPR_RSHIFT):
			return eval(binaryExpr->getLeftExpr(), flag) >> eval(binaryExpr->getRightExpr(), flag);

		case(astNode::E_EXPR_BITWAND):
			return eval(binaryExpr->getLeftExpr(), flag) & eval(binaryExpr->getRightExpr(), flag);

		case(astNode::E_EXPR_BITWOR):
			return eval(binaryExpr->getLeftExpr(), flag) | eval(binaryExpr->getRightExpr(), flag);

		case(astNode::E_EXPR_BITWXOR):
			return eval(binaryExpr->getLeftExpr(), flag) ^ eval(binaryExpr->getRightExpr(), flag);

		case(astNode::E_EXPR_BITWNEG):
			return ~eval(unaryExpr->getExpr(), flag);

		case(astNode::E_EXPR_COND):
		{
			auto cond = dynamic_cast<const exprCond*>(node);
			if(eval(cond->getCond(), EVAL_EXPRESSION) > 0)
				return eval(cond->getThen(), flag);
			else
				return eval(cond->getElse(), flag);
		}

		case(astNode::E_EXPR_LEN):
		{
			auto varRef = dynamic_cast<const exprUnary*>(node)->getExpr();
			return getChannel(getVarName(varRef))->len();
		}

		case(astNode::E_EXPR_CONST):
			return dynamic_cast<const exprConst*>(node)->getCstValue();

		case(astNode::E_EXPR_TIMEOUT):
			return s->getTimeoutStatus();

		case(astNode::E_EXPR_FULL):
		case(astNode::E_EXPR_NFULL):
		{
			assert(false);
			auto varRef = dynamic_cast<const exprUnary*>(node)->getExpr();
			auto chanVar = dynamic_cast<channel*>(varRef);
			return node->getType() == astNode::E_EXPR_FULL? chanVar->full() : !chanVar->full();
		}

		case(astNode::E_EXPR_EMPTY):
		case(astNode::E_EXPR_NEMPTY):
		{
			auto varRef = dynamic_cast<const exprUnary*>(node)->getExpr();
			auto chanVar = getChannel(getVarName(varRef));
			return node->getType() == astNode::E_EXPR_EMPTY? chanVar->empty() : !chanVar->empty();
		}

		case(astNode::E_VARREF):
		{
			auto varRef = dynamic_cast<const exprVarRef*>(node);
			return get<scalarInt*>(getVarName(varRef))->getIntValue();
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
 * Returns a list of all the executable transitions (for all the processes).
 * EFFECTS: None. (It CANNOT have any!)
 * WARNING:
 * 	In the end, does NOT (and must NEVER) modify the state payload.
 */
std::list<transition*> process::executables(void) const {

	std::list<transition*> res;

	const fsmNode* node = getFsmNodePointer();
	
	if(!node) 
		return res;


	auto s = getProgState();
	if(s->hasExclusivity() && s->getExclusiveProcId() != getPid())
		return res;

	for(auto edge : node->getEdges()) {

		if(eval(edge, EVAL_EXECUTABILITY) > 0) {

			//auto conjunct = s->getFeatures() * edge->getFeatures();

			if(edge->getExpression()->getType() == astNode::E_STMNT_CHAN_SND) {
				
				auto cSendStmnt = dynamic_cast<const stmntChanSnd*>(edge->getExpression());
				auto chan = getChannel(getVarName(cSendStmnt->getChan()));
				chan->send(getInputParamList(cSendStmnt->getArgList()));


				// channelSend has modified the value of HANDSHAKE and one other byte in the payload.
				// These two will have to get back their original value.
				// Also, channelSend has allocated memory to handshake_transit: it will have to be free'd.

				if(chan->isRendezVous()) {
					
					// If the sender had the exclusivity, it lost it because of the rendezvous completion.
					// Proceed in automaton
					auto hadExclusivity = s->getExclusiveProc() == this;
					if(hadExclusivity)
						s->resetExclusivity();
					
					// Applying the response transition.
					// Note that the features of the resulting state will be: "state->features & request_transition->features & response_transition->features"
					// Also, applying this transition will
					std::list<transition*> responses = s->executables();
				
					// assert(responses.size() > 0);
					// After the recursive call, each transition in e_ is executable and its features satisfy the modified base FD.
					// featuresOut contains all the outgoing features from now on, included the ones of the response that satisfy the base FD (those may not satisfy the modified FD, though).
					// *allProductsOut == 1 if the outgoing features reference all the products.


					for(auto response : responses) {
						//conjunct *= dynamic_cast<progTransition*>(response)->getEdge()->getFeatures();
						//if((conjunct * s->stateMachine->getFD()).IsOne())
						//res.push_back(new RVTransition(s, const_cast<process*>(this), edge, conjunct, dynamic_cast<progTransition*>(response)));
						assert(dynamic_cast<threadTransition*>(response) != nullptr);
					
						auto question = initState::createProcTransition(const_cast<process*>(this), edge);
						auto rdv = new rendezVousTransition(s, question, response);
						res.push_back(rdv);
					}

					chan->pop();
					// Rendezvous completed: HANDSHAKE is reset.
					s->resetHandShake();

					// Put Back the exclusivity if it was there.
					if(hadExclusivity)
						s->setExclusivity(this);
				}
			
			} else if (edge->getExpression()->getType() == astNode::E_STMNT_CHAN_RCV) {

				auto recv = initState::createProcTransition(const_cast<process*>(this), edge);

				auto cSendStmnt = dynamic_cast<const stmntChanRecv*>(edge->getExpression());
				auto chan = getChannel(getVarName(cSendStmnt->getChan()));

				// channelSend has modified the value of HANDSHAKE and one other byte in the payload.
				// These two will have to get back their original value.
				// Also, channelSend has allocated memory to handshake_transit: it will have to be free'd.
				if(chan->isRendezVous()) {
					res.push_back(recv);
				} else {
					res.push_back(new programTransition(s, recv));
				}

			} else { 

				//to wrap/abstract when I will have time
				//if((conjunct * s->stateMachine->getFD()).IsOne())
					//res.push_back(new progTransition(s, const_cast<process*>(this), edge, conjunct));
				//assert(edge->getExpression()->getType() != astNode::E_STMNT_CHAN_RCV || !edge->getFeatures());

				res.push_back(new programTransition(s, initState::createProcTransition(const_cast<process*>(this), edge)));
			}
		}
	}

	if(res.size() == 0 && !_else) {
		_else = 1;
		res = executables();
		_else = 0;
	}

	return res;
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
void process::apply(transition* trans) {
	auto threadTrans = dynamic_cast<const threadTransition*>(trans);
	auto proc = dynamic_cast<const process*>(threadTrans->getThread());
	auto edge =  threadTrans->getEdge();

	assert(proc && proc->getPid() == getPid());
	assert(edge);

	auto expression = edge->getExpression();

	//auto oldLocation = getLocation();
	//_assertViolation = 0;

	program* s = getProgState();

	//std::cout << " APPLYING LINE: " << *(trans->lines.cbegin()) << " to process " << this->getFullName() << std::endl;

	byte leaveUntouched = 0; // Set to 1 in case of a rendez-vous channel send.
Apply:

	switch(expression->getType()) {
		case(astNode::E_VAR_DECL):
		case(astNode::E_STMNT):
			assert(false);
			break;

		case(astNode::E_STMNT_CHAN_SND):
		{
			// Sends the message in the correct channel.
			// Increases by one unit the number of messages of this channel.
			// If the channel was a rendezvous channel, _handshake_transit has been allocated.
			auto sndStmnt = dynamic_cast<const stmntChanSnd*>(expression);
			auto chan = getChannel(getVarName(sndStmnt->getChan()));
			chan->send(getInputParamList(sndStmnt->getArgList()));

			if(chan->isRendezVous()) {
				leaveUntouched = 1;

				//assert(trans->getResponses().size() > 0);

				// Send was a rendezvous request. We immediately try to complete this rendezvous.
				s->setHandShake({chan, this});
				
				// If the sender had the exclusivity, it lost it because of the rendezvous completion.
				if(s->getExclusiveProc() == this)
					s->resetExclusivity();

				// Proceed in automaton
				setFsmNodePointer(edge->getTargetNode());

				// Applying the response transition.
				// Note that the features of the resulting state will be: "state->features & request_transition->features & response_transition->features"
				// Also, applying this transition will free _handshake_transit (because of calling "channelReceive()").
				// Furthermore, the number of message in the rendezvous channel will be 0.
				
				/*for(auto r : trans->getResponses()) {
					dynamic_cast<progTransition*>(r)->getProc()->apply(r);
				}*/
				
				// Rendezvous completed: HANDSHAKE is reset.
			}
			break;
		}

		case(astNode::E_STMNT_CHAN_RCV):
		{
			auto recvStmnt = dynamic_cast<const stmntChanRecv*>(expression);
			auto chan = getChannel(getVarName(recvStmnt->getChan()));
			
			if(chan->isRendezVous())
				assert(s->hasHandShakeRequest() && s->getHandShakeRequestChan() == chan);
			else 
				assert(!s->hasHandShakeRequest());

			assert(!chan->empty() || chan->isRendezVous());

			auto rargs = getOutputParamList(recvStmnt->getRArgList());
			assert(chan->isReceivable(rargs));
			chan->receive(rargs);
			chan->pop();
			if(s->getHandShakeRequestChan() == chan)
				s->resetHandShake();
			// If there was a rendezvous request, it has been accepted.
			break;
		}
		
		case(astNode::E_STMNT_IF):
		case(astNode::E_STMNT_DO):
		case(astNode::E_STMNT_OPT):
		case(astNode::E_STMNT_SEQ):
		case(astNode::E_STMNT_LABEL):
			//failure("Found control statement while applying an expression at line %2d\n", expression->lineNb);
			assert(false);
			break;

		case(astNode::E_STMNT_ACTION):
		{
			auto action = dynamic_cast<const stmntAction*>(expression);
			getProgState()->actions.push_back(action->getLabel());
		}
		case(astNode::E_STMNT_BREAK):
		case(astNode::E_STMNT_GOTO):
			break;

		case(astNode::E_STMNT_ASGN):
		{
			auto assign = dynamic_cast<const stmntAsgn*>(expression);
			expr* rvalue = assign->getAssign();
			auto value = eval(rvalue, EVAL_EXPRESSION);
			auto* lvalue = get<scalarInt*>(getVarName(assign->getVarRef()));
			lvalue->setIntValue(value);
			break;
		}
		case(astNode::E_STMNT_INCR):
		{
			auto incr = dynamic_cast<const stmntIncr*>(expression);
			auto var = get<scalarInt*>(getVarName(incr->getVarRef()));
			var->setIntValue(var->getIntValue() + 1);
			break;
		}
		case(astNode::E_STMNT_DECR):
		{
			auto incr = dynamic_cast<const stmntDecr*>(expression);
			auto var = get<scalarInt*>(getVarName(incr->getVarRef()));
			var->setIntValue(var->getIntValue() - 1);
			break;
		}
		case(astNode::E_STMNT_PRINT):
			//executePromelaPrint(globalSymTab, mtypes, expression->sVal, expression->children[0], state, process);
			assert(false);
			break;

		case(astNode::E_STMNT_PRINTM): 
		{
				/*int value;
				if(expression->children[0]) {
					symbol = expressionSymbolLookUpLeft(expression->children[0]);
					field = expressionSymbolLookUpRight(expression->children[0]);
					if(symbol->global == 0)
						offset = getVarOffset(globalSymTab, mtypes, copy, process, process->offset, expression->children[0]);
					else
						offset = getVarOffset(globalSymTab, mtypes, copy, process, 0, expression->children[0]);
					value = stateGetValue(copy->payload, offset, field->type);
				} else {
					value = expression->iVal;
				}*/
				assert(false);
				break;
		}

		case(astNode::E_STMNT_ELSE):
			break;

		case(astNode::E_STMNT_ASSERT):
		{
			auto assertExpr = dynamic_cast<const stmntAssert*>(expression);
			if(eval(assertExpr->getToAssert(), EVAL_EXPRESSION) == 0) {
				std::cout << "Assertion failed process : "<< getName() <<"@"<< assertExpr->getLineNb()<< std::endl;
				exit(1);
				//if(!_assertViolation) _assertViolation = 1;
			}
			//assert(false);
			break;
		}
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

		case(astNode::E_EXPR_RUN):
		{
			// A new process can run iff MAX_PROCESS processes or less are currently running.
			/*auto runExpr = dynamic_cast<const exprRun*>(expression);
			
			auto newProc = initState::createProcess(s->stateMachine, runExpr->getProcType(), 0, 0);

			auto argIt = getArgs(runExpr->getArgList()).cbegin();			
			for(auto argSym : dynamic_cast<const ptypeSymNode*>(runExpr->getProcType())->getArgs()){
				auto vars = initState::addVariables(newProc, argSym);
				assert(vars.size() == 1);
				auto var = vars.begin();
				*dynamic_cast<primitiveVariable*>(*var++) = *argIt++;
			}*/
			assert(false);
			break;
		}

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

	// All of this is not needed in case of a rendez-vous
	if(!leaveUntouched) {
		
		// Proceed in automaton
		
		setFsmNodePointer(edge->getTargetNode());

		// Set exclusivity of process that fired the atomic transition.
		if(isAtomic()) 
			s->setExclusivity(this);
		else {
			s->resetExclusivity();
		}
		s->lastStepPid = proc->getPid();
	}

	//TODO : compute the prob

	origin = trans;
	// Make sure that we do not override the destination of another transition.
	assert(trans->dst == nullptr || trans->dst == this);
	trans->dst = this;

	//std::cout << this->getFullName() << "::apply (" << oldLocation << ", " << dynamic_cast<const processTransition*>(trans)->getEdge()->getLineNb() << ", " << getLocation() << ")" << std::endl;
}

void process::print(void) const {
	auto node = getFsmNodePointer();
	if(node) 	printf("%s pid  %-13u @ NL%02u\n", getFullName().c_str(), getPid(), node->getLineNb());
	else 		printf("%s pid  %-13u @ end\n", getFullName().c_str(), getPid());

	variable::print();
}

void process::printCSVHeader(std::ostream &out) const {
	out << getFullName() << ",";
}

void process::printCSV(std::ostream &out) const {
	out << getLocation() << ",";
}

void process::accept(stateVisitor* visitor) {
	visitor->visit(this);
}