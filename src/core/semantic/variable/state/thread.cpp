#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "thread.hpp"

#include "rendezVousTransition.hpp"
#include "paramList.hpp"
#include "transition.hpp"

#include "payload.hpp"
#include "variable.hpp"

#include "scalarVar.hpp"
#include "pidVar.hpp"

#include "ast.hpp"
#include "automata.hpp"

#include "initState.hpp"

#include "pidVar.hpp"

thread::thread(variable::Type type, const std::string& name, const fsmNode* start, ubyte pid)
	: state(type, name)
	, start(start)
  , loc(start)
	, _else(false)
  , pid(pid)
{
}

thread::thread(const thread& other)
	: state(other)
	, start(other.start)
  , loc(other.loc)
	, _else(other._else)
  , pid(other.pid)
{}

void thread::init(void) {
	//assert(getProgState());

	variable::init();
	setFsmNodePointer(start);
}

void thread::setPid(ubyte pid) {
  this->pid = pid;
}

std::vector<std::shared_ptr<statePredicate>> thread::getPredicates(void) const{
  return variable::getPredicates();
}


ubyte thread::getPid(void) const {
	return pid;
}

const fsmNode * thread::getFsmNodePointer(void) const { return loc; }

void thread::setFsmNodePointer(const fsmNode * pointer) { this->loc = pointer; }

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

paramList thread::getOutputParamList(const exprRArgList * rargs) const
{
  paramList res;
  while (rargs) {
    auto exp = rargs->getExprRArg();
    if(exp->getType() == astNode::E_RARG_VAR)
      res.push_back(new paramRef(get<scalarInt*>(getVarName(exp))));
    else
      res.push_back(new paramValue(eval(exp, EVAL_EXPRESSION)));
    rargs = rargs->getRArgList();
  }
  return res;
}

paramList thread::getInputParamList(const exprArgList * args) const
{
  paramList res;
  while (args) {
    auto exp = args->getExprArg()->getExpr();
    res.push_back(new paramValue(eval(exp, EVAL_EXPRESSION)));
    args = args->getArgList();
  }
  return res;
}

/*variable * thread::getVariableFromExpr(const expr * varExpr) const
{
  auto varName = getVarName(varExpr);
  return get(varName);
}*/

/**
 * @brief Get the arguments of a channel send.
 * get the variables or the values of the arguments of a channel send.
 * @param args
 * @return std::list<arg> 
*/
/*
argList thread::getArgs(const exprArgList * args) const
{
  std::list<arg> res;
  while (args) {
    auto exp = args->getExprArg()->getExpr();
    if (exp->getType() == astNode::E_EXPR_CONST)
      res.push_back(arg(eval(exp, EVAL_EXPRESSION)));
    else
      res.push_back(arg(dynamic_cast<variable*> (getVariableFromExpr(exp))));
    args = args->getArgList();
  }
  return res;
}*/

/**
 * @brief Get the arguments of a channel receive.
 * get the variables or the values of the arguments of a channel receive.
 * variables are used to store the values of the received arguments.
 * values are used to compare the received arguments with the expected ones.
 * values should be equal to the received arguments to be executed
 * @param rargs
 * @return std::list<arg> 
*/

/*argList thread::getRArgs(const exprRArgList * rargs) const
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
      res.push_back(arg(dynamic_cast<variable*>(getVariableFromExpr(exp))));
      break;
    default:
      assert(false);
      break;
    }
    rargs = rargs->getRArgList();
  }
  return res;
}*/

//channel * thread::getChannelFromExpr(const expr * varExpr) const { return variable::getChannel(getVarName(varExpr)); }

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
  return getFsmNodePointer()->getLineNb() == cast->getFsmNodePointer()->getLineNb();
}

bool thread::operator!=(const variable * other) const { return !(*this == other); }

variable * thread::operator=(const variable * other)
{
  variable::operator=(other);
  auto cast = dynamic_cast<const thread *>(other);
  if(cast)
    setFsmNodePointer(cast->getFsmNodePointer());
  else 
    assert(false);
  return this;
}

void thread::printGraphViz(unsigned long i) const {}

float thread::delta(const variable * s2, bool considerInternalVariables) const
{
  auto cast = dynamic_cast<const thread *>(s2);
  if(!cast)
    return 1;
  auto delta =  variable::delta(s2, considerInternalVariables) * 0.5;

  auto node = getFsmNodePointer();
  auto otherNode = cast->getFsmNodePointer();

  if (cast) {
    int lineNb = node ? node->getLineNb() : -1;
    int otherLineNb = otherNode ? otherNode->getLineNb() : -1;
    delta += (lineNb != otherLineNb) ? 0.5 : 0;
  }
  assert(delta >= 0 && delta <= 1);
  return delta;
}

void thread::printDelta(const variable * v2, bool considerInternalVariables) const
{
  auto cast = dynamic_cast<const thread *>(v2);
  if (cast == nullptr)
    return;
  variable::printDelta(v2, considerInternalVariables);

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

size_t thread::size(void) const { return variable::size() + sizeof(loc); }

void thread::hash(byte * payload) const
{
  auto locLine = loc->getLineNb( );
  memcpy(payload, &locLine, sizeof(locLine));
  variable::hash(payload + sizeof(locLine));
}

std::list<variable *> thread::getDelta(const variable * v2, bool considerInternalVariables) const
{
  std::list<variable *> res;
  auto cast = dynamic_cast<const thread *>(v2);
  if (cast == nullptr)
    return res;

  res = variable::getDelta(v2, considerInternalVariables);
  return res;
}

unsigned long thread::hash(void) const
{
  return variable::hash();
}