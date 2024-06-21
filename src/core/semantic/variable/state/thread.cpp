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

thread::thread(variable::Type type, const seqSymNode * sym, const fsmNode * start, byte pid, unsigned int index)
    : state(type, sym->getName()), symType(sym), index(index), start(start), _else(false), pid(pid)
{

  // seq sym node need boud attr. if arrays 
  // Why do we need to assert that the index is 0?
  assert(index == 0);

  addRawBytes(sizeof(const fsmNode *));
}

thread::thread(const thread & other)
    : state(other), symType(other.symType), index(other.index), start(other.start), _else(other._else), pid(other.pid)
{
}

thread::thread(const thread * other)
    : state(other), symType(other->symType), index(other->index), start(other->start), _else(other->_else), pid(other->pid)
{
}

void thread::init(void)
{
  // assert(getProgState());

  variable::init();
  setFsmNodePointer(start);

  variable::getTVariable<PIDVar *>("_pid")->setValue(pid);
}

std::vector<std::shared_ptr<statePredicate>> thread::getPredicates(void) const{
  return variable::getPredicates();
}


byte thread::getPid(void) const { return payLoad ? variable::getTVariable<PIDVar *>("_pid")->getValue() : pid; }

void thread::setPid(byte pid)
{
  this->pid = pid;
  if (payLoad)
    variable::getTVariable<PIDVar *>("_pid")->setValue(pid);
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

std::list<variable *> thread::getVariables(const exprArgList * args) const
{
  std::list<variable *> res;
  while (args) {
    auto exp = args->getExprArg()->getExpr();
    variable * ptr;
    if (exp->getType() == astNode::E_EXPR_CONST)
      ptr = new constVar(eval(exp, EVAL_EXPRESSION), variable::getVarType(exp->getExprType()), exp->getLineNb());
    else
      ptr = getVariable(exp);
    res.push_back(ptr);
    args = args->getArgList();
  }
  return res;
}

std::list<const variable *> thread::getConstVariables(const exprArgList * args) const
{
  std::list<const variable *> res;
  while (args) {
    auto exp = args->getExprArg()->getExpr();
    const variable * ptr;
    if (exp->getType() == astNode::E_EXPR_CONST)
      ptr = new constVar(eval(exp, EVAL_EXPRESSION), variable::getVarType(exp->getExprType()), exp->getLineNb());
    else
      ptr = getVariable(exp);
    res.push_back(ptr);
    args = args->getArgList();
  }
  return res;
}

std::list<variable *> thread::getVariables(const exprRArgList * rargs) const
{
  std::list<variable *> res;
  while (rargs) {
    auto exp = rargs->getExprRArg();
    variable * ptr;
    switch (exp->getType()) {
    case astNode::E_RARG_CONST:
    case astNode::E_RARG_EVAL:
      ptr = new constVar(eval(exp, EVAL_EXPRESSION), variable::getVarType(exp->getExprType()), exp->getLineNb());
      break;
    case astNode::E_RARG_VAR:
      ptr = getVariable(exp);
      break;
    default:
      assert(false);
      break;
    }
    res.push_back(ptr);
    rargs = rargs->getRArgList();
  }
  return res;
}

std::list<const variable *> thread::getConstVariables(const exprRArgList * rargs) const
{
  std::list<const variable *> res;
  while (rargs) {
    auto exp = rargs->getExprRArg();
    const variable * ptr;
    switch (exp->getType()) {
    case astNode::E_RARG_CONST:
    case astNode::E_RARG_EVAL:
      ptr = new constVar(eval(exp, EVAL_EXPRESSION), variable::getVarType(exp->getExprType()), exp->getLineNb());
      break;
    case astNode::E_RARG_VAR:
      ptr = getVariable(exp)->deepCopy();
      break;
    default:
      assert(false);
      break;
    }
    res.push_back(ptr);
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

float thread::delta(const variable * s2, bool considerInternalVariables) const
{
  auto cast = dynamic_cast<const thread *>(s2);
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

std::list<variable *> thread::getDelta(const variable * v2, bool considerInternalVariables) const
{
  std::list<variable *> res;
  auto cast = dynamic_cast<const thread *>(v2);
  if (cast == nullptr)
    return res;

  res = variable::getDelta(v2, considerInternalVariables);
  return res;
}