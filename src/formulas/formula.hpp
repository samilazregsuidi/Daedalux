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

  virtual std::vector<std::string> getDefinitions() const = 0;

  std::string getDefinitionString() const
  {
    auto definitions = getDefinitions();
    return std::accumulate(definitions.begin(), definitions.end(), std::string(),
                           [](const std::string & a, const std::string & b) -> std::string { return a + b + "\n"; });
  }

  virtual std::string getDefName() const = 0;

  virtual int getDepth() const = 0;

  virtual int getSize() const = 0;

  virtual bool isEquivalent(const formula & other) const = 0;

  virtual std::string promelaFormula() const = 0;

  // Define equality based on your criteria
  bool operator==(const formula & other) const { return this->isEquivalent(other); }

  std::string neverClaim() const
  {
    auto ltl_formula = promelaFormula();
    return LTLClaimsProcessor::transformLTLStringToNeverClaim(ltl_formula);
  }

protected:
  std::string sanitizeName(const std::string & definition) const
  {
    auto defName = definition;
    std::replace(defName.begin(), defName.end(), ' ', '_');
    std::replace(defName.begin(), defName.end(), '.', '_');
    std::replace(defName.begin(), defName.end(), '[', '_');
    std::replace(defName.begin(), defName.end(), ']', '_');
    return defName;
  }
};

// Custom hash function
namespace std {
template <> struct hash<formula> {
  std::size_t operator()(const formula & f) const
  {
    // Placeholder for actual hash computation
    // For simplicity, we're directly hashing the expression string
    return std::hash<std::string>()(f.toFormula());
  }
};
} // namespace std

class LeafFormula : public formula {
public:
  std::vector<std::string> getDefinitions() const { throw std::runtime_error("Leaf formulas do not have a definition"); }

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

  bool isEquivalent(const formula & other) const
  {
    auto otherVariable = dynamic_cast<const VariableFormula *>(&other);
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

  bool isEquivalent(const formula & other) const
  {
    auto otherNumber = dynamic_cast<const NumberConstant *>(&other);
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

  bool isEquivalent(const formula & other) const
  {
    auto otherBoolean = dynamic_cast<const BooleanConstant *>(&other);
    if (!otherBoolean) {
      return false;
    }
    return value == otherBoolean->value;
  }

  std::string promelaFormula() const { return toFormula(); }

  bool isTrue() const { return value; }

private:
  bool value;
};

class UnaryFormula : public formula {
public:
  UnaryFormula(const std::shared_ptr<formula> & subformula) : subformula(subformula) {}
  ~UnaryFormula() = default;

  std::string toFormula() const { return getOperator() + " " + subformula->toFormula(); }

  std::string promelaFormula() const { return getOperator() + " " + subformula->promelaFormula(); }

  std::vector<std::string> getDefinitions() const { return subformula->getDefinitions(); }

  int getDepth() const { return subformula->getDepth() + 1; }

  std::string getDefName() const { return subformula->getDefName(); }

  int getSize() const { return subformula->getSize() + 1; }

  std::shared_ptr<formula> getSubformula() const { return subformula; }

  bool isEquivalent(const formula & other) const
  {
    auto otherUnary = dynamic_cast<const UnaryFormula *>(&other);
    if (!otherUnary) {
      return false;
    }
    return getOperator() == otherUnary->getOperator() && subformula->isEquivalent(*otherUnary->getSubformula());
  }

  void setSubformula(const std::shared_ptr<formula> & newSubformula) { subformula = newSubformula; }

private:
  virtual std::string getOperator() const = 0;
  std::shared_ptr<formula> subformula;
};

class ParenthesisFormula : public formula {
public:
  ParenthesisFormula(const std::shared_ptr<formula> & subformula) : subformula(std::move(subformula)) {}
  ~ParenthesisFormula() = default;

  std::string toFormula() const { return "(" + subformula->toFormula() + ")"; }

  std::string promelaFormula() const { return "(" + subformula->promelaFormula() + ")"; }

  int getDepth() const { return subformula->getDepth() + 1; }

  int getSize() const { return subformula->getSize() + 1; }
  std::vector<std::string> getDefinitions() const { return subformula->getDefinitions(); }

  std::string getDefName() const { return subformula->getDefName(); }

  bool isEquivalent(const formula & other) const
  {
    const ParenthesisFormula * otherParenthesis = dynamic_cast<const ParenthesisFormula *>(&other);
    if (!otherParenthesis) {
      return false;
    }
    return subformula->isEquivalent(*otherParenthesis->subformula);
  }

private:
  std::shared_ptr<formula> subformula;
};

class NotFormula : public UnaryFormula {
public:
  NotFormula(const std::shared_ptr<formula> & subformula) : UnaryFormula(std::move(subformula)) {}
  ~NotFormula() = default;

private:
  std::string getOperator() const { return "!"; }
};

class NextFormula : public UnaryFormula {
public:
  NextFormula(const std::shared_ptr<formula> & subformula) : UnaryFormula(std::move(subformula))
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
  GloballyFormula(const std::shared_ptr<formula> & subformula) : UnaryFormula(std::move(subformula))
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
  FinallyFormula(const std::shared_ptr<formula> & subformula) : UnaryFormula(std::move(subformula))
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

  std::vector<std::string> getDefinitions() const
  {
    auto leftDef = left->getDefinitions();
    auto rightDef = right->getDefinitions();
    leftDef.insert(leftDef.end(), rightDef.begin(), rightDef.end());
    // Remove duplicates
    std::sort(leftDef.begin(), leftDef.end());
    leftDef.erase(std::unique(leftDef.begin(), leftDef.end()), leftDef.end());
    return leftDef;
  }

  std::string getDefName() const { return left->getDefName() + "\n" + right->getDefName(); }

  std::string promelaFormula() const { return left->promelaFormula() + " " + getOperator() + " " + right->promelaFormula(); }

  std::shared_ptr<formula> getLeft() const { return left; }
  std::shared_ptr<formula> getRight() const { return right; }

  int getDepth() const { return std::max(left->getDepth(), right->getDepth()) + 1; }

  int getSize() const { return left->getSize() + right->getSize() + 1; }

  bool isEquivalent(const formula & other) const
  {
    auto otherBinary = dynamic_cast<const BinaryFormula *>(&other);
    if (!otherBinary) {
      return false;
    }
    auto sameOperator = getOperator() == otherBinary->getOperator();
    auto sameLeft = left->isEquivalent(*otherBinary->getLeft());
    auto sameRight = right->isEquivalent(*otherBinary->getRight());
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
  bool isEquivalent(const formula & other) const
  {
    auto otherBinary = dynamic_cast<const IffFormula *>(&other);
    if (!otherBinary) {
      return false;
    }
    auto sameOperator = getOperator() == otherBinary->getOperator();
    auto sameOrderEqual =
        getLeft()->isEquivalent(*otherBinary->getLeft()) && getRight()->isEquivalent(*otherBinary->getRight());
    auto otherOrderEqual =
        getLeft()->isEquivalent(*otherBinary->getRight()) && getRight()->isEquivalent(*otherBinary->getLeft());
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
  bool isEquivalent(const formula & other) const
  {
    auto otherBinary = dynamic_cast<const AndFormula *>(&other);
    if (!otherBinary) {
      return false;
    }
    auto sameOperator = getOperator() == otherBinary->getOperator();
    auto sameOrderEqual =
        getLeft()->isEquivalent(*otherBinary->getLeft()) && getRight()->isEquivalent(*otherBinary->getRight());
    auto otherOrderEqual =
        getLeft()->isEquivalent(*otherBinary->getRight()) && getRight()->isEquivalent(*otherBinary->getLeft());
    bool isEquivalent = sameOperator && (sameOrderEqual || otherOrderEqual);
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
  bool isEquivalent(const formula & other) const
  {
    const OrFormula * otherBinary = dynamic_cast<const OrFormula *>(&other);
    if (!otherBinary) {
      return false;
    }
    auto sameOperator = getOperator() == otherBinary->getOperator();
    auto sameOrderEqual =
        getLeft()->isEquivalent(*otherBinary->getLeft()) && getRight()->isEquivalent(*otherBinary->getRight());
    auto otherOrderEqual =
        getLeft()->isEquivalent(*otherBinary->getRight()) && getRight()->isEquivalent(*otherBinary->getLeft());
    bool isEquivalent = sameOperator && (sameOrderEqual || otherOrderEqual);
    return isEquivalent;
  }

  std::string getOperator() const { return "||"; }
};

class ComparisonFormula : public BinaryFormula {
public:
  ComparisonFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : BinaryFormula(std::move(left), std::move(right))
  {
  }
  ~ComparisonFormula() = default;

  std::string promelaFormula() const { return getDefName(); }

  std::vector<std::string> getDefinitions() const
  {
    auto entry = "#define " + getDefName() + " (" + toFormula() + ")";
    return {entry};
  }

  std::string getDefName() const
  {
    auto left_name = getLeft()->toFormula();
    auto right_name = getRight()->toFormula();
    auto defName = left_name + "_" + getComparisionName() + "_" + right_name;
    return sanitizeName(defName);
  }

  virtual std::string getComparisionName() const = 0;
};

class LargerThanFormula : public ComparisonFormula {
public:
  LargerThanFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : ComparisonFormula(std::move(left), std::move(right))
  {
  }
  ~LargerThanFormula() = default;

  std::string getOperator() const { return ">"; }

  std::string getComparisionName() const { return "larger_than"; }
};

class SmallerThanFormula : public ComparisonFormula {
public:
  SmallerThanFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : ComparisonFormula(std::move(left), std::move(right))
  {
  }
  ~SmallerThanFormula() = default;

  std::string getOperator() const { return "<"; }

  std::string getComparisionName() const { return "smaller_than"; }
};

class LargerEqualsFormula : public ComparisonFormula {
public:
  LargerEqualsFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : ComparisonFormula(std::move(left), std::move(right))
  {
  }

  ~LargerEqualsFormula() = default;

  std::string getOperator() const { return ">="; }

  std::string getComparisionName() const { return "larger_equals"; }
};

class SmallerEqualsFormula : public ComparisonFormula {
public:
  SmallerEqualsFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : ComparisonFormula(std::move(left), std::move(right))
  {
  }
  ~SmallerEqualsFormula() = default;

  std::string getOperator() const { return "<="; }
  std::string getComparisionName() const { return "smaller_equals"; }
};

class EqualsFormula : public ComparisonFormula {
public:
  EqualsFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : ComparisonFormula(std::move(left), std::move(right))
  {
  }
  ~EqualsFormula() = default;

  std::string toFormula() const
  {
    auto leftChild = getLeft();
    auto rightChild = getRight();
    // Find the type of the children
    auto leftVariable = std::dynamic_pointer_cast<VariableFormula>(leftChild);
    auto rightVariable = std::dynamic_pointer_cast<VariableFormula>(rightChild);
    if (leftVariable && rightVariable) {
      return leftVariable->toFormula() + " == " + rightVariable->toFormula();
    }
    else if (leftVariable && !rightVariable) {
      auto isBoolean = std::dynamic_pointer_cast<BooleanConstant>(rightChild);
      if (isBoolean) {
        auto defName = getDefName();
        auto leftName = leftChild->toFormula();
        if (isBoolean->isTrue()) {
          return leftName;
        }
        else {
          return "!" + leftName;
        }
      }
      else {
        return leftChild->toFormula() + " == " + rightChild->toFormula();
      }
    }
    else if (!leftVariable && rightVariable) {
      auto isBoolean = std::dynamic_pointer_cast<BooleanConstant>(leftChild);
      if (isBoolean) {
        if (isBoolean->isTrue()) {
          return rightChild->toFormula();
        }
        else {
          return "!" + rightChild->toFormula();
        }
      }
      else {
        return leftChild->toFormula() + " == " + rightChild->toFormula();
      }
    }
    else if (!leftVariable && !rightVariable) {
      throw std::runtime_error("Not implemented");
    }
    return getDefName();
  }

  std::string promelaFormula() const
  {
    auto leftChild = getLeft();
    auto rightChild = getRight();
    // Find the type of the children
    auto leftVariable = std::dynamic_pointer_cast<VariableFormula>(leftChild);
    auto rightVariable = std::dynamic_pointer_cast<VariableFormula>(rightChild);
    if (leftVariable && rightVariable) {
      return getDefName();
    }
    else if (leftVariable && !rightVariable) {
      auto isBoolean = std::dynamic_pointer_cast<BooleanConstant>(rightChild);
      if (isBoolean) {
        return getDefName();
        auto leftName = leftChild->promelaFormula();
        if (isBoolean->isTrue()) {
          return leftName;
        }
        else {
          return "!" + leftName;
        }
      }
      else {
        auto defName = getDefName();
        return defName;
      }
    }
    else if (!leftVariable && rightVariable) {
      auto isBoolean = std::dynamic_pointer_cast<BooleanConstant>(leftChild);
      if (isBoolean) {
        if (isBoolean->isTrue()) {
          return rightChild->promelaFormula();
        }
        else {
          return "!" + rightChild->promelaFormula();
        }
      }
      else {
        return leftChild->promelaFormula() + " == " + rightChild->promelaFormula();
      }
    }
    else if (!leftVariable && !rightVariable) {
      throw std::runtime_error("Not implemented");
    }
    return getDefName();
  }

  std::string getOperator() const { return "=="; }
  std::string getComparisionName() const { return "equals"; }
};

class NotEqualsFormula : public ComparisonFormula {
public:
  NotEqualsFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : ComparisonFormula(std::move(left), std::move(right))
  {
  }
  ~NotEqualsFormula() = default;

  std::string getOperator() const { return "!="; }
  std::string getComparisionName() const { return "not_equals"; }
};
