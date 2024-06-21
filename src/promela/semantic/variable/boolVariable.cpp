#include "boolVariable.hpp"
#include <cstdio>
#include "../../src/formulas/predicates/statePredicate.hpp"
#include "../../src/formulas/predicates/binaryPredicate.hpp"
#include "../../src/formulas/predicates/valuesPredicate.hpp"


#include "boolSymNode.hpp"

boolVar::boolVar(const boolSymNode * sym, unsigned int index) : primitiveVariable(sym, index) {}

variable * boolVar::deepCopy(void) const { return new boolVar(*this); }

int boolVar::operator++(void) { assert(false); }

int boolVar::operator--(void) { assert(false); }

int boolVar::operator++(int) { assert(false); }

int boolVar::operator--(int) { assert(false); }

bool boolVar::operator==(bool value) const { return getValue() == value; }

bool boolVar::operator!=(bool value) const { return getValue() != value; }

boolVar::operator bool(void) const { return getValue() != 0 ? true : false; }

std::vector<std::shared_ptr<statePredicate>> boolVar::getPredicates() const
{
  auto varName = getLocalName();
  auto predVar = std::make_shared<VariablePredicate<bool>>(varName);
  bool value = getValue() ? true : false;
  auto predValue = std::make_shared<ConstantPredicate<bool>>(value);
  auto predName = varName + "_equals_" + std::to_string(value);
  auto pred = std::make_shared<EqualityPredicate<bool>>(predName, predVar, predValue);
  std::vector<std::shared_ptr<statePredicate>> predicates;
  predicates.push_back(pred);
  return predicates;
}


float boolVar::delta(const variable * other, bool _) const
{
  auto otherVar = dynamic_cast<const boolVar *>(other);
  if (!otherVar)
    return 1;
  return (getValue() != otherVar->getValue()) ? 1 : 0;
}

void boolVar::printDelta(const variable * other, bool _) const
{
  auto otherVar = dynamic_cast<const boolVar *>(other);
  if (!otherVar)
    return;
  if (getValue() != otherVar->getValue())
    printf("%s: %s -> %s\n", getFullName().c_str(), getValue() ? "true" : "false", otherVar->getValue() ? "true" : "false");
}

boolVar::operator std::string(void) const
{
  char buffer[128];
  if (getValue() == 1)
    snprintf(buffer, sizeof(buffer), "0x%-4lx:   %-23s = true\n", getOffset(), getFullName().c_str());
  else
    snprintf(buffer, sizeof(buffer), "0x%-4lx:   %-23s = false\n", getOffset(), getFullName().c_str());
  return buffer;
}

void boolVar::print(void) const { printf("%s", std::string(*this).c_str()); }

void boolVar::printTexada(void) const
{
  if (varSym->isPredefined())
    return;

  auto val = getValue() ? "true" : "false";
  printf("%s = %s\n", getFullName().c_str(), val);
}

void boolVar::printCSVHeader(std::ostream & out) const
{
  if (varSym->isPredefined())
    return;
  out << getFullName() << ",";
}

void boolVar::printCSV(std::ostream & out) const
{
  if (varSym->isPredefined())
    return;
  auto val = getValue() ? "true" : "false";
  out << val << ",";
}