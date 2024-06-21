#ifndef BOOL_VARIABLE_H
#define BOOL_VARIABLE_H

#include "primitiveVariable.hpp"

class boolSymNode;

class boolVar : public primitiveVariable {
public:
  boolVar(const boolSymNode * sym, unsigned int index = 0);

  variable * deepCopy(void) const override;

  ~boolVar() override {}

  int operator++(void) override;

  int operator--(void) override;

  int operator++(int) override;

  int operator--(int) override;

  bool operator==(bool value) const;

  bool operator!=(bool value) const;

  std::vector<std::shared_ptr<statePredicate>> getPredicates() const override;

  float delta(const variable * other, bool considerInternalVariables) const override;

  void printDelta(const variable * other, bool considerInternalVariables) const override;

  operator std::string(void) const override;

  operator bool(void) const;

  void print(void) const override;

  void printTexada(void) const override;

  void printCSV(std::ostream & out) const override;

  void printCSVHeader(std::ostream & out) const override;
};

#endif