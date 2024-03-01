#pragma once

#include "ltl.hpp"
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class formula {
public:
  virtual std::string toFormula() const = 0;

  virtual std::string getDefinition() const = 0;

  virtual std::string getDefName() const = 0;

  virtual int getDepth() const = 0;

  virtual int getSize() const = 0;

  virtual bool isEquivalent(const std::shared_ptr<formula> & other) const = 0;

  virtual std::string promelaFormula() const = 0;

  std::string neverClaim() const
  {
    auto ltl_formula = promelaFormula();
    return LTLClaimsProcessor::transformLTLStringToNeverClaim(ltl_formula);
  }
};

class LeafFormula : public formula {
public:
  std::string getDefinition() const { throw std::runtime_error("Leaf formulas do not have a definition"); }

  std::string getDefName() const { throw std::runtime_error("Leaf formulas do not have a definition name"); }

  int getDepth() const { return 1; }

  int getSize() const { return 1; }
};

class VariableFormula : public LeafFormula {
public:
  VariableFormula(const std::string & name) : name(name)
  {
    if (name.empty()) {
      throw std::invalid_argument("Variable name cannot be empty");
    }
  }

  ~VariableFormula() = default;

  std::string toFormula() const { return name; }

  std::string promelaFormula() const { return name; }

  bool isEquivalent(const std::shared_ptr<formula> & other) const
  {
    auto otherVariable = std::dynamic_pointer_cast<VariableFormula>(other);
    if (!otherVariable) {
      return false;
    }
    return name == otherVariable->name;
  }

private:
  std::string name;
};

class NumberConstant : public LeafFormula {
public:
  NumberConstant(int value) : value(value) {}
  ~NumberConstant() = default;

  std::string toFormula() const { return std::to_string(value); }

  bool isEquivalent(const std::shared_ptr<formula> & other) const
  {
    auto otherNumber = std::dynamic_pointer_cast<NumberConstant>(other);
    if (!otherNumber) {
      return false;
    }
    return value == otherNumber->value;
  }

  std::string promelaFormula() const { return toFormula(); }

private:
  int value;
};

class BooleanConstant : public LeafFormula {
public:
  BooleanConstant(bool value) : value(value) {}
  ~BooleanConstant() = default;

  std::string toFormula() const { return value ? "true" : "false"; }

  bool isEquivalent(const std::shared_ptr<formula> & other) const
  {
    auto otherBoolean = std::dynamic_pointer_cast<BooleanConstant>(other);
    if (!otherBoolean) {
      return false;
    }
    return value == otherBoolean->value;
  }

  std::string promelaFormula() const { return toFormula(); }

private:
  bool value;
};

class UnaryFormula : public formula {
public:
  UnaryFormula(const std::shared_ptr<formula> & subformula) : subformula(subformula) {}
  ~UnaryFormula() = default;

  std::string toFormula() const { return getOperator() + " " + subformula->toFormula(); }

  std::string promelaFormula() const { return getOperator() + " " + subformula->promelaFormula(); }

  std::string getDefinition() const { return subformula->getDefinition(); }

  int getDepth() const { return subformula->getDepth() + 1; }

  std::string getDefName() const { return subformula->getDefName(); }

  int getSize() const { return subformula->getSize() + 1; }

  std::shared_ptr<formula> getSubformula() const { return subformula; }

  bool isEquivalent(const std::shared_ptr<formula> & other) const
  {
    auto otherUnary = std::dynamic_pointer_cast<UnaryFormula>(other);
    if (!otherUnary) {
      return false;
    }
    return getOperator() == otherUnary->getOperator() && subformula->isEquivalent(otherUnary->getSubformula());
  }

  void setSubformula(const std::shared_ptr<formula> & newSubformula) { subformula = newSubformula; }

private:
  virtual std::string getOperator() const = 0;
  std::shared_ptr<formula> subformula;
};

class ParenthesisFormula : public formula {
public:
  ParenthesisFormula(const std::shared_ptr<formula> & subformula) : subformula(subformula) {}
  ~ParenthesisFormula() = default;

  std::string toFormula() const { return "(" + subformula->toFormula() + ")"; }

  std::string promelaFormula() const { return "(" + subformula->promelaFormula() + ")"; }

  int getDepth() const { return subformula->getDepth() + 1; }

  int getSize() const { return subformula->getSize() + 1; }
  std::string getDefinition() const { return subformula->getDefinition(); }

  std::string getDefName() const { return subformula->getDefName(); }

  bool isEquivalent(const std::shared_ptr<formula> & other) const
  {
    auto otherParenthesis = std::dynamic_pointer_cast<ParenthesisFormula>(other);
    if (!otherParenthesis) {
      return false;
    }
    return subformula->isEquivalent(otherParenthesis->subformula);
  }

private:
  std::shared_ptr<formula> subformula;
};

class NotFormula : public UnaryFormula {
public:
  NotFormula(const std::shared_ptr<formula> & subformula) : UnaryFormula(subformula) {}
  ~NotFormula() = default;

private:
  std::string getOperator() const { return "!"; }
};

class NextFormula : public UnaryFormula {
public:
  NextFormula(const std::shared_ptr<formula> & subformula) : UnaryFormula(subformula)
  {
    // Cast the subformula to a ParenthesisFormula if it is not already one
    auto subformulaAsParenthesis = std::dynamic_pointer_cast<ParenthesisFormula>(subformula);
    if (!subformulaAsParenthesis) {
      setSubformula(std::make_shared<ParenthesisFormula>(subformula));
    }
  }
  ~NextFormula() = default;

private:
  std::string getOperator() const { return "X"; }
};

class GloballyFormula : public UnaryFormula {
public:
  GloballyFormula(const std::shared_ptr<formula> & subformula) : UnaryFormula(subformula)
  {
    // Cast the subformula to a ParenthesisFormula if it is not already one
    auto subformulaAsParenthesis = std::dynamic_pointer_cast<ParenthesisFormula>(subformula);
    if (!subformulaAsParenthesis) {
      setSubformula(std::make_shared<ParenthesisFormula>(subformula));
    }
  }

  ~GloballyFormula() = default;

private:
  std::string getOperator() const { return "[]"; }
};

class FinallyFormula : public UnaryFormula {
public:
  FinallyFormula(const std::shared_ptr<formula> & subformula) : UnaryFormula(subformula)
  {
    // Cast the subformula to a ParenthesisFormula if it is not already one
    auto subformulaAsParenthesis = std::dynamic_pointer_cast<ParenthesisFormula>(subformula);
    if (!subformulaAsParenthesis) {
      setSubformula(std::make_shared<ParenthesisFormula>(subformula));
    }
  }

  ~FinallyFormula() = default;

private:
  std::string getOperator() const { return "<>"; }
};

class BinaryFormula : public formula {
public:
  BinaryFormula(const std::shared_ptr<formula> & left, const std::shared_ptr<formula> & right) : left(left), right(right) {}
  ~BinaryFormula() = default;

  std::string toFormula() const { return left->toFormula() + " " + getOperator() + " " + right->toFormula(); }

  std::string getDefinition() const { return left->getDefinition() + "\n" + right->getDefinition(); }

  std::string getDefName() const { return left->getDefName() + "\n" + right->getDefName(); }

  std::string promelaFormula() const { return left->promelaFormula() + " " + getOperator() + " " + right->promelaFormula(); }

  std::shared_ptr<formula> getLeft() const { return left; }
  std::shared_ptr<formula> getRight() const { return right; }

  int getDepth() const { return std::max(left->getDepth(), right->getDepth()) + 1; }

  int getSize() const { return left->getSize() + right->getSize() + 1; }

  bool isEquivalent(const std::shared_ptr<formula> & other) const
  {
    auto otherBinary = std::dynamic_pointer_cast<BinaryFormula>(other);
    if (!otherBinary) {
      return false;
    }
    auto sameOperator = getOperator() == otherBinary->getOperator();
    auto sameLeft = left->isEquivalent(otherBinary->getLeft());
    auto sameRight = right->isEquivalent(otherBinary->getRight());
    return sameOperator && sameLeft && sameRight;
  }

  void setLeft(const std::shared_ptr<formula> & newLeft) { left = newLeft; }
  void setRight(const std::shared_ptr<formula> & newRight) { right = newRight; }

  virtual std::string getOperator() const = 0;

private:
  std::shared_ptr<formula> left;
  std::shared_ptr<formula> right;
};

class ImpliesFormula : public BinaryFormula {
public:
  ImpliesFormula(const std::shared_ptr<formula> & left, const std::shared_ptr<formula> & right) : BinaryFormula(left, right)
  {
    // Cast the subformula to a ParenthesisFormula if it is not already one
    auto leftAsParenthesis = std::dynamic_pointer_cast<ParenthesisFormula>(left);
    if (!leftAsParenthesis) {
      setLeft(std::make_shared<ParenthesisFormula>(left));
    }
    auto rightAsParenthesis = std::dynamic_pointer_cast<ParenthesisFormula>(right);
    if (!rightAsParenthesis) {
      setRight(std::make_shared<ParenthesisFormula>(right));
    }
  }
  ~ImpliesFormula() = default;

private:
  std::string getOperator() const { return "->"; }
};

class IffFormula : public BinaryFormula {
public:
  IffFormula(const std::shared_ptr<formula> & left, const std::shared_ptr<formula> & right) : BinaryFormula(left, right)
  {
    // Cast the subformula to a ParenthesisFormula if it is not already one
    auto leftAsParenthesis = std::dynamic_pointer_cast<ParenthesisFormula>(left);
    if (!leftAsParenthesis) {
      setLeft(std::make_shared<ParenthesisFormula>(left));
    }
    auto rightAsParenthesis = std::dynamic_pointer_cast<ParenthesisFormula>(right);
    if (!rightAsParenthesis) {
      setRight(std::make_shared<ParenthesisFormula>(right));
    }
  }
  ~IffFormula() = default;

  // The order of the operands is NOT important for the equivalence of the formulas
  bool isEquivalent(const std::shared_ptr<formula> & other) const
  {
    auto otherBinary = std::dynamic_pointer_cast<IffFormula>(other);
    if (!otherBinary) {
      return false;
    }
    auto sameOperator = getOperator() == otherBinary->getOperator();
    auto sameOrderEqual = getLeft()->isEquivalent(otherBinary->getLeft()) && getRight()->isEquivalent(otherBinary->getRight());
    auto otherOrderEqual = getLeft()->isEquivalent(otherBinary->getRight()) && getRight()->isEquivalent(otherBinary->getLeft());
    return sameOperator && (sameOrderEqual || otherOrderEqual);
  }

private:
  std::string getOperator() const { return "<->"; }
};

class AndFormula : public BinaryFormula {
public:
  AndFormula(const std::shared_ptr<formula> & left, const std::shared_ptr<formula> & right) : BinaryFormula(left, right) {}
  ~AndFormula() = default;

  // The order of the operands is NOT important for the equivalence of the formulas
  bool isEquivalent(const std::shared_ptr<formula> & other) const
  {
    auto otherBinary = std::dynamic_pointer_cast<AndFormula>(other);
    if (!otherBinary) {
      return false;
    }
    auto sameOperator = getOperator() == otherBinary->getOperator();
    auto sameOrderEqual = getLeft()->isEquivalent(otherBinary->getLeft()) && getRight()->isEquivalent(otherBinary->getRight());
    auto otherOrderEqual = getLeft()->isEquivalent(otherBinary->getRight()) && getRight()->isEquivalent(otherBinary->getLeft());
    bool isEquivalent = sameOperator && (sameOrderEqual || otherOrderEqual);
    if (!isEquivalent) {
      std::cout << "Not equivalent: " << toFormula() << " and " << other->toFormula() << std::endl;
    }
    return isEquivalent;
  }

private:
  std::string getOperator() const { return "&&"; }
};

class OrFormula : public BinaryFormula {
public:
  OrFormula(const std::shared_ptr<formula> & left, const std::shared_ptr<formula> & right) : BinaryFormula(left, right) {}
  ~OrFormula() = default;

  // The order of the operands is NOT important for the equivalence of the formulas
  bool isEquivalent(const std::shared_ptr<formula> & other) const
  {
    auto otherBinary = std::dynamic_pointer_cast<OrFormula>(other);
    if (!otherBinary) {
      return false;
    }
    auto sameOperator = getOperator() == otherBinary->getOperator();
    auto sameOrderEqual = getLeft()->isEquivalent(otherBinary->getLeft()) && getRight()->isEquivalent(otherBinary->getRight());
    auto otherOrderEqual = getLeft()->isEquivalent(otherBinary->getRight()) && getRight()->isEquivalent(otherBinary->getLeft());
    bool isEquivalent = sameOperator && (sameOrderEqual || otherOrderEqual);
    if (!isEquivalent) {
      std::cout << "Not equivalent: " << toFormula() << " and " << other->toFormula() << std::endl;
    }
    return isEquivalent;
  }

  std::string getOperator() const { return "||"; }
};

class ComparisonFormula : public BinaryFormula {
public:
  ComparisonFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : BinaryFormula(left, right)
  {
  }
  ~ComparisonFormula() = default;

  std::string promelaFormula() const { return getDefName(); }

  std::string getDefinition() const { return "#define " + getDefName() + " (" + toFormula() + ")"; }

  std::string getDefName() const
  {
    auto left_name = getLeft()->toFormula();
    auto right_name = getRight()->toFormula();
    auto defName = left_name + "_" + getComparisionName() + "_" + right_name;
    std::replace(defName.begin(), defName.end(), ' ', '_');
    std::replace(defName.begin(), defName.end(), '-', '_');
    std::replace(defName.begin(), defName.end(), '[', '_');
    std::replace(defName.begin(), defName.end(), ']', '_');
    return defName;
  }

  virtual std::string getComparisionName() const = 0;
};

class LargerThanFormula : public ComparisonFormula {
public:
  LargerThanFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : ComparisonFormula(left, right)
  {
  }
  ~LargerThanFormula() = default;

  std::string getOperator() const { return ">"; }

  std::string getComparisionName() const { return "larger_than"; }
};

class SmallerThanFormula : public ComparisonFormula {
public:
  SmallerThanFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : ComparisonFormula(left, right)
  {
  }
  ~SmallerThanFormula() = default;

  std::string getOperator() const { return "<"; }

  std::string getComparisionName() const { return "smaller_than"; }
};

class LargerEqualsFormula : public ComparisonFormula {
public:
  LargerEqualsFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : ComparisonFormula(left, right)
  {
  }

  ~LargerEqualsFormula() = default;

  std::string getOperator() const { return ">="; }

  std::string getComparisionName() const { return "larger_equals"; }
};

class SmallerEqualsFormula : public ComparisonFormula {
public:
  SmallerEqualsFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : ComparisonFormula(left, right)
  {
  }
  ~SmallerEqualsFormula() = default;

  std::string getOperator() const { return "<="; }
  std::string getComparisionName() const { return "smaller_equals"; }
};

class EqualsFormula : public ComparisonFormula {
public:
  EqualsFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : ComparisonFormula(left, right)
  {
  }
  ~EqualsFormula() = default;

  std::string getOperator() const { return "=="; }
  std::string getComparisionName() const { return "equals"; }
};

class NotEqualsFormula : public ComparisonFormula {
public:
  NotEqualsFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : ComparisonFormula(left, right)
  {
  }
  ~NotEqualsFormula() = default;

  std::string getOperator() const { return "!="; }
  std::string getComparisionName() const { return "not_equals"; }
};
