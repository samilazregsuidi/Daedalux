#include "mtypeVariable.hpp"

#include "varExpr.hpp"
#include "../../src/formulas/predicates/statePredicate.hpp"
#include "../../src/formulas/predicates/unaryPredicate.hpp"
#include "../../src/formulas/predicates/binaryPredicate.hpp"
#include "../../src/formulas/predicates/valuesPredicate.hpp"

mtypeVar::mtypeVar(const mtypeSymNode * sym, unsigned int index) : primitiveVariable(sym, index) { assert(varType == V_MTYPE); }

void mtypeVar::init(void)
{

  auto initExpr = varSym->getInitExpr();

  if (initExpr) {

    // its a cmtype value...
    assert(initExpr->getType() == astNode::E_EXPR_VAR);

    auto sym = dynamic_cast<exprVar *>(initExpr)->getFinalSymbol();
    assert(sym && sym->getType() == symbol::T_CMTYPE);

    setValue(dynamic_cast<const cmtypeSymNode *>(sym)->getIntValue());
  }
}

int mtypeVar::operator++(void) { assert(false); }

int mtypeVar::operator--(void) { assert(false); }

int mtypeVar::operator++(int) { assert(false); }

int mtypeVar::operator--(int) { assert(false); }

bool mtypeVar::operator==(const std::string & cmtype) const
{
  auto def = dynamic_cast<const mtypeSymNode *>(varSym)->getMTypeDef();
  if (!def) {
    assert(false);
    return false;
  }
  auto cmtypeValue = def->getCmtypeSymNodeValue(cmtype);
  assert(cmtypeValue != -1);
  return (getValue() == cmtypeValue);
}

bool mtypeVar::operator!=(const std::string & cmtype) const { return !(*this == cmtype); }

std::vector<std::shared_ptr<statePredicate>> mtypeVar::getPredicates(void) const
{
  auto varName = getLocalName();
  auto varNamePred = std::make_shared<VariablePredicate<std::string>>(varName);
  auto valueNamePred = std::make_shared<ConstantPredicate<std::string>>(getValueName());
  auto predName = varName + "_equals_" + getValueName();
  auto pred = std::make_shared<EqualityPredicate<std::string>>(predName, varNamePred, valueNamePred);
  std::vector<std::shared_ptr<statePredicate>> predicates;
  predicates.push_back(pred);
  return predicates;
}

float mtypeVar::delta(const variable * other, bool considerInternalVariables) const
{
  auto otherVar = dynamic_cast<const mtypeVar *>(other);
  if (!otherVar) {
    // Not the same type
    return 1;
  }
  auto value = getValue();
  auto otherValue = otherVar->getValue();
  auto hasSameValue = (value == otherValue);
  if (hasSameValue) {
    return 0;
  }
  auto valueName = getValueName();
  auto otherValueName = otherVar->getValueName();
  auto hasSameValueName = (valueName.compare(otherValueName) == 0);
  if (hasSameValueName) {
    // Hack for the case where the same name is used for different values
    return 0;
  }
  if (hasSameValueName && !hasSameValue) {
    auto def = dynamic_cast<const mtypeSymNode *>(varSym)->getMTypeDef();
    auto def_other = dynamic_cast<const mtypeSymNode *>(otherVar->varSym)->getMTypeDef();
    auto offset = def->getIntValueOffset();
    auto offset_other = def_other->getIntValueOffset();
    auto value_minus_offset = value - offset;
    auto value_minus_offset_other = otherValue - offset_other;
    hasSameValue = (value_minus_offset == value_minus_offset_other);
  }
  return (hasSameValue && hasSameValueName) ? 0 : 1;
}

std::string mtypeVar::getValueName(void) const
{
  auto def = dynamic_cast<const mtypeSymNode *>(varSym)->getMTypeDef();
  if (def) {
    auto value = getValue();
    auto mtypestr = def->getCmtypeSymNodeName(value);
    return mtypestr;
  }
  else {
    return std::to_string(getValue());
  }
}

void mtypeVar::printDelta(const variable * other, bool considerInternalVariables) const
{
  if (isSame(other, considerInternalVariables))
    return;

  auto otherVar = dynamic_cast<const mtypeVar *>(other);
  if (!otherVar)
    return;

  auto name = getFullName();
  auto valueName = getValueName();
  auto otherValueName = otherVar->getValueName();
  printf("%s: %s -> %s\n", name.c_str(), valueName.c_str(), otherValueName.c_str());
}

std::list<variable *> mtypeVar::getDelta(const variable * other, bool considerInternalVariables) const
{
  std::list<variable *> res;
  if (isSame(other, considerInternalVariables))
    return res;

  auto otherVar = dynamic_cast<const mtypeVar *>(other);
  if (!otherVar)
    return res;

  res.push_back(deepCopy());
  return res;
}

variable * mtypeVar::deepCopy(void) const
{
  mtypeVar * copy = new mtypeVar(*this);
  // warning shared payload!
  return copy;
}

mtypeVar::operator std::string(void) const
{
  char buffer[128];
  auto value = getValue();
  if (value) {
    auto valueName = getValueName();
    sprintf(buffer, "0x%-4lx:   %-23s = %s\n", getOffset(), getFullName().c_str(), valueName.c_str());
  }
  else {
    sprintf(buffer, "0x%-4lx:   %-23s = nil\n", getOffset(), getFullName().c_str());
  }
  return buffer;
}

void mtypeVar::print(void) const { printf("%s", std::string(*this).c_str()); }

void mtypeVar::printTexada(void) const
{
  if (varSym->isPredefined())
    return;
  auto value = getValue();
  if (value) {
    auto valueName = getValueName();
    printf("%s = %s\n", getFullName().c_str(), valueName.c_str());
  }
  else {
    printf("%s = nil\n", getFullName().c_str());
  }
}

void mtypeVar::printCSV(std::ostream & out) const
{
  if (varSym->isPredefined())
    return;
  auto value = getValue();
  if (value) {
    auto valueName = getValueName();
    out << getFullName() + " = " + valueName << std::endl;
  }
  else {
    out << getFullName() + " = nil" << std::endl;
  }
}

void mtypeVar::printCSVHeader(std::ostream & out) const
{
  if (varSym->isPredefined())
    return;
  auto value = getValue();
  if (value) {
    auto valueName = getValueName();
    out << getFullName() + " = " + valueName << std::endl;
  }
  else {
    out << getFullName() + " = nil" << std::endl;
  }
}

/******************************************************************************************************/

cmtypeVar::cmtypeVar(const cmtypeSymNode * sym) : primitiveVariable(sym) {}

void cmtypeVar::setValue(int value) { assert(false); }

int cmtypeVar::getValue(void) const { return dynamic_cast<const cmtypeSymNode *>(varSym)->getIntValue(); }

variable * cmtypeVar::deepCopy(void) const
{
  cmtypeVar * copy = new cmtypeVar(*this);
  // warning shared payload!
  return copy;
}

int cmtypeVar::operator=(const primitiveVariable & rvalue)
{
  rvalue;
  assert(false);
}

int cmtypeVar::operator++(void) { assert(false); }

int cmtypeVar::operator--(void) { assert(false); }

int cmtypeVar::operator++(int) { assert(false); }

int cmtypeVar::operator--(int) { assert(false); }

cmtypeVar::operator std::string() const
{
  return std::string();
  // assert(false);
}

void cmtypeVar::print(void) const
{
  // assert(false);
}

void cmtypeVar::printTexada(void) const {}

void cmtypeVar::printCSV(std::ostream & out) const {}

void cmtypeVar::printCSVHeader(std::ostream & out) const {}

/******************************************************************************************************/