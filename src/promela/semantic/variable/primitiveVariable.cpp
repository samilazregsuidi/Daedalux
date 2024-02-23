#include "primitiveVariable.hpp"
#include <cstdio>

#include "constExpr.hpp"
#include "payload.hpp"
#include "symbols.hpp"

primitiveVariable::primitiveVariable(const varSymNode * const varSym, unsigned int index)
    : variable(variable::getVarType(varSym->getType()),
               std::string(
                   varSym ? (varSym->getBound() > 1 ? varSym->getName() + "[" + std::to_string(index) + "]" : varSym->getName())
                          : "")),
      varSym(varSym), index(index)
{
  isPredef = varSym->isHidden();
  isHidden = varSym->isPredefined();
  // assert(varSym && (varSym->getType() == symbol::T_INT || varSym->getType() == symbol::T_BIT || varSym->getType() ==
  // symbol::T_BYTE || varSym->getType() == symbol::T_SHORT));
}

primitiveVariable::primitiveVariable(Type varType, unsigned int index) : variable(varType), varSym(nullptr), index(index)
{
  // assert(varSym && (varSym->getType() == symbol::T_INT || varSym->getType() == symbol::T_BIT || varSym->getType() ==
  // symbol::T_BYTE || varSym->getType() == symbol::T_SHORT));
  assert(false);

  if (index > 0)
    name += "[" + std::to_string(index) + "]";
}

primitiveVariable::primitiveVariable(const primitiveVariable & other)
    : variable(other), varSym(other.varSym), index(other.index)
{
}

primitiveVariable::primitiveVariable(const primitiveVariable * other)
    : variable(other), varSym(other->varSym), index(other->index)
{
}

variable * primitiveVariable::deepCopy(void) const { return new primitiveVariable(this); }

void primitiveVariable::init(void)
{

  auto initExpr = varSym->getInitExpr();

  if (initExpr) {
    auto initExprConst = dynamic_cast<exprConst *>(initExpr);
    // init expr should be const and const only!
    assert(initExprConst);
    setValue(initExprConst->getCstValue());
  }

  variable::init();
}

bool primitiveVariable::isGlobal(void) const { return varSym->isGlobal(); }

size_t primitiveVariable::getSizeOf(void) const { return varSym->getTypeSize(); }

int primitiveVariable::operator=(const primitiveVariable & rvalue)
{
  int res = rvalue.getValue();
  setValue(res);
  return res;
}

int primitiveVariable::operator=(const arg & rvalue)
{
  switch (rvalue.type) {
  case arg::VAL:
    setValue(rvalue.data.value);
    break;

  case arg::VAR:
    auto var = dynamic_cast<const primitiveVariable *>(rvalue.data.variable);
    assert(var);
    setValue(rvalue.data.variable->getValue());
    break;
  }
}

int primitiveVariable::operator++(void)
{
  auto temp = getValue();
  auto bound = varSymNode::getUpperBound(varSym->getType());
  if (temp < bound) {
    setValue(temp + 1);
    return temp + 1;
  }
  return temp;
}

int primitiveVariable::operator--(void)
{
  auto temp = getValue();
  if (temp - 1 >= varSymNode::getLowerBound(varSym->getType())) {
    setValue(temp - 1);
    return temp - 1;
  }
  return temp;
}

int primitiveVariable::operator++(int)
{
  auto temp = getValue();
  if (temp + 1 <= varSymNode::getUpperBound(varSym->getType()))
    setValue(temp + 1);
  return temp;
}

int primitiveVariable::operator--(int)
{
  auto temp = getValue();
  if (temp - 1 >= varSymNode::getLowerBound(varSym->getType()))
    setValue(temp - 1);
  return temp;
}

bool primitiveVariable::operator==(const variable * other) const
{
  auto cast = dynamic_cast<const primitiveVariable *>(other);
  if (!cast)
    return false;
  return getValue() == cast->getValue();
}

bool primitiveVariable::operator==(int value) const { return getValue() == value; }

bool primitiveVariable::operator!=(const variable * other) const { return !(*this == other); }

bool primitiveVariable::operator!=(int value) const { return !(*this == value); }

float primitiveVariable::delta(const variable * other) const
{
  auto cast = dynamic_cast<const primitiveVariable *>(other);
  if (!cast)
    return 1;

  float value = getPayload()->getValue(getOffset(), getType());
  float otherValue = cast->getPayload()->getValue(cast->getOffset(), cast->getType());

  float diff = std::abs(value - otherValue);
  auto delta = 1.0 - (1.0 / (diff + 1.0));
  return delta;
}

void primitiveVariable::printDelta(const variable * other) const
{
  auto cast = dynamic_cast<const primitiveVariable *>(other);
  if (!cast)
    return;

  auto delta = this->delta(cast);

  if (delta > 0.00000001) {
    auto name = getFullName();
    auto value = getPayload()->getValue(getOffset(), getType());
    auto otherValue = cast->getPayload()->getValue(cast->getOffset(), cast->getType());
    auto OtherName = cast->getFullName();
    printf("%s = %d, %s = %d, delta = %f\n", name.c_str(), value, OtherName.c_str(), otherValue, delta);
  }
}

std::list<variable *> primitiveVariable::getDelta(const variable * other) const
{
  std::list<variable *> res;
  auto cast = dynamic_cast<const primitiveVariable *>(other);
  if (!cast)
    return res;

  auto delta = this->delta(cast);

  if (delta > 0.00000001) {
    res.push_back(this->deepCopy());
  }
  return res;
}

void primitiveVariable::setValue(int value)
{
  assert(getPayload());
  assert(value >= varSymNode::getLowerBound(varSym->getType()));
  assert(value <= varSymNode::getUpperBound(varSym->getType()));
  getPayload()->setValue(getOffset(), value, getType());
  assert(getValue() == value);
}

int primitiveVariable::getValue(void) const
{
  assert(getPayload());
  auto value = getPayload()->getValue(getOffset(), getType());
  // assert(value >= varSymNode::getLowerBound(varSym->getType()) && value <= varSymNode::getUpperBound(varSym->getType()));
  return value;
}

//primitiveVariable::operator int(void) const { return getValue(); }

void primitiveVariable::reset(void) { setValue(0); }

primitiveVariable::operator ::std::string(void) const
{
  assert(getPayload());
  auto value = getPayload()->getValue(getOffset(), getType());
  char buffer[128];
  snprintf(buffer, sizeof(buffer), "0x%-4ld:   %-23s = %d\n", getOffset(), getFullName().c_str(), value);

  // res += variable::operator std::string();
  return buffer;
}

void primitiveVariable::print(void) const
{
  auto name = std::string(*this);
  printf("%s", name.c_str());
}

void primitiveVariable::printTexada(void) const
{
  assert(getPayload());
  if (varSym->isPredefined())
    return;

  auto value = getPayload()->getValue(getOffset(), getType());
  printf("%s = %d\n", getFullName().c_str(), value);

  variable::printTexada();
}

void primitiveVariable::printCSVHeader(std::ostream & out) const
{
  assert(getPayload());
  if (varSym->isPredefined())
    return;

  out << getFullName() + ",";
  variable::printCSVHeader(out);
}

void primitiveVariable::printCSV(std::ostream & out) const
{
  assert(getPayload());
  if (varSym->isPredefined())
    return;

  auto value = getPayload()->getValue(getOffset(), getType());
  out << std::to_string(value) + ",";

  variable::printCSV(out);
}

/*************************************************************************************************/

constVar::constVar(int value, variable::Type type, int lineNb) : primitiveVariable(type), value(value), lineNb(lineNb)
{
  assert(false); // sizeOf +=
}

void constVar::setValue(int val)
{
  value = val;
  assert(false);
}

int constVar::getValue(void) const { return value; }

int constVar::operator=(const primitiveVariable & rvalue)
{
  value = rvalue.getValue();
  assert(false);
}

int constVar::operator++(void) { assert(false); }

int constVar::operator--(void) { assert(false); }

int constVar::operator++(int) { assert(false); }

int constVar::operator--(int) { assert(false); }

variable * constVar::deepCopy(void) const
{
  constVar * copy = new constVar(*this);
  // warning shared payload!
  return copy;
}

/******************************************************************************************************/

#include "process.hpp"

PIDVar::PIDVar(const pidSymNode * sym, unsigned int bound) : primitiveVariable(sym, bound), ref(nullptr) {}

variable * PIDVar::deepCopy(void) const
{
  variable * copy = new PIDVar(*this);
  return copy;
}

process * PIDVar::getRefProcess(void) const { return ref; }

void PIDVar::setRefProcess(process * newRef)
{
  ref = newRef;
  getPayload()->setValue<byte>(getOffset(), newRef->getPid());
}

void PIDVar::assign(const variable * sc)
{
  variable::assign(sc);
  if (ref) {
    ref = getTVariable<process *>(ref->getLocalName());
    assert(ref);
  }
}