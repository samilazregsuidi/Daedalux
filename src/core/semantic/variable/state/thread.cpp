#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "program.hpp"
#include "rendezVousTransition.hpp"
#include "thread.hpp"
#include "transition.hpp"

#include "channel.hpp"
#include "payload.hpp"
#include "variable.hpp"

#include "ast.hpp"
#include "automata.hpp"

#include "initState.hpp"

thread::thread(variable::Type type, const seqSymNode* sym, const fsmNode* start, byte pid, unsigned int index)
	: state(type, sym->getName())
	, symType(sym)
	, index(index)
	, start(start)
	, _else(false)
	, pid(pid)
{

  // seq sym node need boud attr. if arrays
  assert(index == 0);

  addRawBytes(sizeof(const fsmNode *));
}

thread::thread(const thread& other)
	: state(other)
	, symType(other.symType)
	, index(other.index)
	, start(other.start)
	, _else(other._else)
	, pid(other.pid)
{}

thread::thread(const thread* other)
	: state(other)
	, symType(other->symType)
	, index(other->index)
	, start(other->start)
	, _else(other->_else)
	, pid(other->pid)
{
}

void thread::init(void) {
	//assert(getProgState());

	variable::init();
	setFsmNodePointer(start);

	variable::getTVariable<PIDVar*>("_pid")->setValue(pid);
}

byte thread::getPid(void) const {
	return payLoad? variable::getTVariable<PIDVar*>("_pid")->getValue() : pid;
}

void thread::setPid(byte pid) {
	this->pid = pid;
	if(payLoad)
		variable::getTVariable<PIDVar*>("_pid")->setValue(pid);
}

const fsmNode * thread::getFsmNodePointer(void) const { return getPayload()->getValue<const fsmNode *>(getOffset()); }

void thread::setFsmNodePointer(const fsmNode * pointer) { getPayload()->setValue<const fsmNode *>(getOffset(), pointer); }

int thread::getLocation(void) const
{
  auto node = getFsmNodePointer();
  return node ? node->getLineNb() : -1;
}

bool thread::isAtLabel(int nbLine) const { return getFsmNodePointer() ? getFsmNodePointer()->getLineNb() == nbLine : false; }

std::string thread::getVarName(const expr * varExpr) const
{
  assert(varExpr->getType() == astNode::E_RARG_VAR || varExpr->getType() == astNode::E_EXPR_VAR ||
         varExpr->getType() == astNode::E_VARREF || varExpr->getType() == astNode::E_VARREF_NAME);

  std::string varName;

  if (varExpr->getType() == astNode::E_RARG_VAR) {
    auto var = dynamic_cast<const exprRArgVar *>(varExpr);
    assert(var);
    return getVarName(var->getVarRef());
  }
  else if (varExpr->getType() == astNode::astNode::E_EXPR_VAR) {
    auto var = dynamic_cast<const exprVar *>(varExpr);
    assert(var);
    return getVarName(var->getVarRef());
  }
  else if (varExpr->getType() == astNode::E_VARREF) {
    auto var = dynamic_cast<const exprVarRef *>(varExpr);
    assert(var);
    varName = getVarName(var->getField());
    return !var->getSubField() ? varName : varName + "." + getVarName(var->getSubField());
  }
  else if (varExpr->getType() == astNode::E_VARREF_NAME) {
    auto varRefName = dynamic_cast<const exprVarRefName *>(varExpr);
    varName = varRefName->getName();
    return varRefName->getIndex() ? varName + "[" + std::to_string(eval(varRefName->getIndex(), EVAL_EXPRESSION)) + "]"
                                  : varName;
  }
  else
    assert(false);

  return varName; // only to please compiler
}

variable * thread::getVariable(const expr * varExpr) const
{
  auto varName = getVarName(varExpr);
  return variable::getVariable(varName);
}

/**
 * @brief Get the arguments of a channel send.
 * get the variables or the values of the arguments of a channel send.
 * @param args
 * @return std::list<arg> 
*/

std::list<arg> thread::getArgs(const exprArgList * args) const
{
  std::list<arg> res;
  while (args) {
    auto exp = args->getExprArg()->getExpr();
    if (exp->getType() == astNode::E_EXPR_CONST)
      res.push_back(arg(eval(exp, EVAL_EXPRESSION)));
    else
      res.push_back(arg(dynamic_cast<primitiveVariable*> (getVariable(exp)->deepCopy())));
    args = args->getArgList();
  }
  return res;
}

/**
 * @brief Get the arguments of a channel receive.
 * get the variables or the values of the arguments of a channel receive.
 * variables are used to store the values of the received arguments.
 * values are used to compare the received arguments with the expected ones.
 * values should be equal to the received arguments to be executed
 * @param rargs
 * @return std::list<arg> 
*/

std::list<arg> thread::getRArgs(const exprRArgList * rargs) const
{
  std::list< arg> res;
  while (rargs) {
    auto exp = rargs->getExprRArg();
    switch (exp->getType()) {
    case astNode::E_RARG_CONST:
    case astNode::E_RARG_EVAL:
      res.push_back(arg(eval(exp, EVAL_EXPRESSION)));
      break;
    case astNode::E_RARG_VAR:
      res.push_back(arg(dynamic_cast<primitiveVariable*> (getVariable(exp)->deepCopy())));
      break;
    default:
      assert(false);
      break;
    }
    rargs = rargs->getRArgList();
  }
  return res;
}

channel * thread::getChannel(const expr * varExpr) const { return variable::getChannel(getVarName(varExpr)); }

bool thread::isAccepting(void) const { return endstate() ? false : getFsmNodePointer()->getFlags() & fsmNode::N_ACCEPT; }

bool thread::isAtomic(void) const { return endstate() ? false : getFsmNodePointer()->getFlags() & fsmNode::N_ATOMIC; }

bool thread::nullstate(void) const { return getFsmNodePointer() == nullptr; }

bool thread::endstate(void) const { return getFsmNodePointer() == nullptr; }

std::string thread::getName(void) const { return variable::getLocalName(); }

bool thread::operator==(const variable * other) const
{
  auto res = variable::operator==(other);
  if (!res)
    return false;
  auto cast = dynamic_cast<const thread *>(other);
  return *getFsmNodePointer() == *cast->getFsmNodePointer();
}

bool thread::operator!=(const variable * other) const { return !(*this == other); }

void thread::printGraphViz(unsigned long i) const {}

float thread::delta(const variable * s2) const
{
  auto cast = dynamic_cast<const thread *>(s2);
  auto delta = variable::delta(s2) * 0.5;
  assert(delta >= 0 && delta <= 1);
  return delta;
}

void thread::printDelta(const variable * v2) const
{
  auto cast = dynamic_cast<const thread *>(v2);
  if (cast == nullptr)
    return;
  variable::printDelta(v2);

  auto node = getFsmNodePointer();
  auto otherNode = cast->getFsmNodePointer();
  if (node == nullptr && otherNode == nullptr)
    return;

  // if (deltaProcess(cast)) {
  //   std::string strLineNbThis = node ? std::to_string(getFsmNodePointer()->getLineNb()) : "end";
  //   std::string strLineNbOther = otherNode ? std::to_string(cast->getFsmNodePointer()->getLineNb()) : "end";
  //   printf("%s @ NL%s -> @ %s\n", getFullName().c_str(), strLineNbThis.c_str(), strLineNbOther.c_str());
  // }
}

std::list<variable *> thread::getDelta(const variable * v2) const
{
  std::list<variable *> res;
  auto cast = dynamic_cast<const thread *>(v2);
  if (cast == nullptr)
    return res;

  res = variable::getDelta(v2);
  return res;
}