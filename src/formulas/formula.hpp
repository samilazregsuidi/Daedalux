#pragma once

#include "ltl.hpp"
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "predicates/statePredicate.hpp"

/**
 * @brief Abstract base class for representing formulas.
 * Provides interfaces for converting to formula string, comparing formulas, etc.
 */
class formula {
public:
  virtual ~formula() = default;

  virtual std::string toFormula() const = 0;

  // Assuming getDefinitions might not be applicable for all formula types
  virtual std::optional<std::vector<std::string>> getDefinitions() const
  {
    return std::nullopt; // Default implementation returns no value
  }

  /// @brief Returns a string with all the definitions of the formula
  /// @return a string with all the definitions of the formula
  std::string getDefinitionString() const
  {
    // Get the definitions or an empty vector if none exist.
    const auto & definitions = getDefinitions().value_or(std::vector<std::string>{});

    std::ostringstream oss;
    for (const auto & def : definitions) {
      oss << def << "\n";
    }
    return oss.str();
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
    std::string sanitized;
    sanitized.reserve(definition.size()); // Reserve space to avoid reallocations
    for (char ch : definition) {
      // Replace with '_' if character matches any of the specified ones
      if (ch == ' ' || ch == '.' || ch == '[' || ch == ']') {
        sanitized += '_';
      }
      else {
        sanitized += ch;
      }
    }
    return sanitized;
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
  std::string getDefName() const override { throw std::runtime_error("Leaf formulas do not have a definition name"); }

  int getDepth() const override { return 1; }

  int getSize() const override { return 1; }
};

class VariableFormula : public LeafFormula {
public:
  explicit VariableFormula(const std::string & name) : name(name)
  {
    if (name.empty()) {
      throw std::invalid_argument("Variable name cannot be empty");
    }
  }

  ~VariableFormula() = default;

  std::string toFormula() const override { return name; }

  std::string promelaFormula() const override { return name; }

  bool isEquivalent(const formula & other) const override
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
  explicit NumberConstant(int value) : value(value) {}
  ~NumberConstant() = default;

  std::string toFormula() const override { return std::to_string(value); }

  bool isEquivalent(const formula & other) const override
  {
    auto otherNumber = dynamic_cast<const NumberConstant *>(&other);
    if (!otherNumber) {
      return false;
    }
    return value == otherNumber->value;
  }

  std::string promelaFormula() const override { return toFormula(); }

private:
  int value;
};

class BooleanConstant : public LeafFormula {
public:
  explicit BooleanConstant(bool value) : value(value) {}
  ~BooleanConstant() = default;

  std::string toFormula() const override { return value ? "true" : "false"; }

  bool isEquivalent(const formula & other) const override
  {
    auto otherBoolean = dynamic_cast<const BooleanConstant *>(&other);
    if (!otherBoolean) {
      return false;
    }
    return value == otherBoolean->value;
  }

  std::string promelaFormula() const override { return toFormula(); }

  bool isTrue() const { return value; }

private:
  bool value;
};

class ParenthesisFormula : public formula {
public:
  explicit ParenthesisFormula(const std::shared_ptr<formula> & subformula) : subformula(std::move(subformula)) {}
  ~ParenthesisFormula() = default;

  std::string toFormula() const override { return "(" + subformula->toFormula() + ")"; }

  std::string promelaFormula() const override { return "(" + subformula->promelaFormula() + ")"; }

  int getDepth() const override { return subformula->getDepth() + 1; }

  int getSize() const override { return subformula->getSize() + 1; }

  std::optional<std::vector<std::string>> getDefinitions() const override { return subformula->getDefinitions(); }

  std::string getDefName() const override { return subformula->getDefName(); }

  bool isEquivalent(const formula & other) const override
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

template <typename T> std::shared_ptr<formula> ensureParenthesis(const std::shared_ptr<T> & subformula)
{
  if (std::dynamic_pointer_cast<ParenthesisFormula>(subformula) == nullptr) {
    return std::make_shared<ParenthesisFormula>(subformula);
  }
  return subformula;
}

class UnaryFormula : public formula {
public:
  explicit UnaryFormula(const std::shared_ptr<formula> & subformula) : subformula(ensureParenthesis(subformula)) {}
  ~UnaryFormula() = default;

  std::string toFormula() const override { return getOperator() + " " + subformula->toFormula(); }

  std::string promelaFormula() const override { return getOperator() + " " + subformula->promelaFormula(); }

  std::optional<std::vector<std::string>> getDefinitions() const override { return subformula->getDefinitions(); }

  int getDepth() const override { return subformula->getDepth() + 1; }

  std::string getDefName() const override { return subformula->getDefName(); }

  int getSize() const override { return subformula->getSize() + 1; }

  std::shared_ptr<formula> getSubformula() const { return subformula; }

  bool isEquivalent(const formula & other) const override
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

class NotFormula : public UnaryFormula {
public:
  explicit NotFormula(const std::shared_ptr<formula> & subformula) : UnaryFormula(std::move(subformula)) {}
  ~NotFormula() = default;

private:
  std::string getOperator() const override { return "!"; }
};

class NextFormula : public UnaryFormula {
public:
  explicit NextFormula(const std::shared_ptr<formula> & subformula) : UnaryFormula(subformula) {}

  ~NextFormula() = default;

private:
  std::string getOperator() const override { return "X"; }
};

class GloballyFormula : public UnaryFormula {
public:
  explicit GloballyFormula(const std::shared_ptr<formula> & subformula) : UnaryFormula(subformula) {}

  ~GloballyFormula() = default;

private:
  std::string getOperator() const override { return "[]"; }
};

class FinallyFormula : public UnaryFormula {
public:
  explicit FinallyFormula(const std::shared_ptr<formula> & subformula) : UnaryFormula(subformula) {}

  ~FinallyFormula() = default;

private:
  std::string getOperator() const override { return "<>"; }
};

class BinaryFormula : public formula {
public:
  BinaryFormula(const std::shared_ptr<formula> & left, const std::shared_ptr<formula> & right) : left(left), right(right) {}

  std::string toFormula() const override { return left->toFormula() + " " + getOperator() + " " + right->toFormula(); }

  std::optional<std::vector<std::string>> getDefinitions() const override
  {
    std::set<std::string> uniqueDefs;
    auto leftDefsOpt = left->getDefinitions();
    if (leftDefsOpt) {
      uniqueDefs.insert(leftDefsOpt->begin(), leftDefsOpt->end());
    }
    auto rightDefsOpt = right->getDefinitions();
    if (rightDefsOpt) {
      uniqueDefs.insert(rightDefsOpt->begin(), rightDefsOpt->end());
    }
    if (uniqueDefs.empty()) {
      return std::nullopt;
    }
    return std::vector<std::string>(uniqueDefs.begin(), uniqueDefs.end());
  }

  std::string getDefName() const override { return left->getDefName() + "\n" + right->getDefName(); }

  std::string promelaFormula() const override
  {
    return left->promelaFormula() + " " + getOperator() + " " + right->promelaFormula();
  }

  std::shared_ptr<formula> getLeft() const { return left; }
  std::shared_ptr<formula> getRight() const { return right; }

  int getDepth() const override { return std::max(left->getDepth(), right->getDepth()) + 1; }

  int getSize() const override { return left->getSize() + right->getSize() + 1; }

  void setLeft(const std::shared_ptr<formula> & newLeft) { left = newLeft; }
  void setRight(const std::shared_ptr<formula> & newRight) { right = newRight; }

  virtual std::string getOperator() const = 0;

  bool isEquivalent(const formula & other) const override
  {
    const auto * o = dynamic_cast<const BinaryFormula *>(&other);
    if (!o || getOperator() != o->getOperator()) {
      // Different types or operators
      return false;
    }

    // Handle commutativity if applicable
    return isCommutative() ? checkCommutativeEquivalence(o) : checkNonCommutativeEquivalence(o);
  }

protected:
  virtual bool isCommutative() const { return false; } // Override in derived classes as needed

private:
  std::shared_ptr<formula> left, right;

  bool checkCommutativeEquivalence(const BinaryFormula * other) const
  {
    return (left->isEquivalent(*other->left) && right->isEquivalent(*other->right)) ||
           (left->isEquivalent(*other->right) && right->isEquivalent(*other->left));
  }

  bool checkNonCommutativeEquivalence(const BinaryFormula * other) const
  {
    return left->isEquivalent(*other->left) && right->isEquivalent(*other->right);
  }
};

class ImpliesFormula : public BinaryFormula {
public:
  ImpliesFormula(const std::shared_ptr<formula> & left, const std::shared_ptr<formula> & right)
      : BinaryFormula(ensureParenthesis(left), ensureParenthesis(right))
  {
  }

  ~ImpliesFormula() = default;

private:
  std::string getOperator() const override { return "->"; }
};

class IffFormula : public BinaryFormula {
public:
  IffFormula(const std::shared_ptr<formula> & left, const std::shared_ptr<formula> & right)
      : BinaryFormula(ensureParenthesis(left), ensureParenthesis(right))
  {
  }

  ~IffFormula() = default;

protected:
  bool isCommutative() const override { return true; }

private:
  std::string getOperator() const override { return "<->"; }
};

class AndFormula : public BinaryFormula {
public:
  AndFormula(const std::shared_ptr<formula> & left, const std::shared_ptr<formula> & right) : BinaryFormula(left, right) {}

  ~AndFormula() = default;

protected:
  bool isCommutative() const override { return true; }

private:
  std::string getOperator() const override { return "&&"; }
};

class OrFormula : public BinaryFormula {
public:
  OrFormula(const std::shared_ptr<formula> & left, const std::shared_ptr<formula> & right) : BinaryFormula(left, right) {}

  ~OrFormula() = default;

  std::string getOperator() const override { return "||"; }

protected:
  bool isCommutative() const override { return true; }
};

class ComparisonFormula : public BinaryFormula {
public:
  ComparisonFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : BinaryFormula(std::move(left), std::move(right))
  {
  }

  ~ComparisonFormula() = default;

  std::string promelaFormula() const override { return getDefName(); }

  std::optional<std::vector<std::string>> getDefinitions() const override
  {
    auto entry = "#define " + getDefName() + " (" + toFormula() + ")";
    return std::vector<std::string>{entry};
  }

  std::string getDefName() const override
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

  std::string getOperator() const override { return ">"; }

  std::string getComparisionName() const override { return "larger_than"; }
};

class SmallerThanFormula : public ComparisonFormula {
public:
  SmallerThanFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : ComparisonFormula(std::move(left), std::move(right))
  {
  }
  ~SmallerThanFormula() = default;

  std::string getOperator() const override { return "<"; }

  std::string getComparisionName() const override { return "smaller_than"; }
};

class LargerEqualsFormula : public ComparisonFormula {
public:
  LargerEqualsFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : ComparisonFormula(std::move(left), std::move(right))
  {
  }

  ~LargerEqualsFormula() = default;

  std::string getOperator() const override { return ">="; }

  std::string getComparisionName() const override { return "larger_equals"; }
};

class SmallerEqualsFormula : public ComparisonFormula {
public:
  SmallerEqualsFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : ComparisonFormula(std::move(left), std::move(right))
  {
  }
  ~SmallerEqualsFormula() = default;

  std::string getOperator() const override { return "<="; }
  std::string getComparisionName() const override { return "smaller_equals"; }
};

class EqualsFormula : public ComparisonFormula {
public:
  EqualsFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : ComparisonFormula(std::move(left), std::move(right))
  {
  }
  ~EqualsFormula() = default;

  std::string toFormula() const override
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

  std::string promelaFormula() const override
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
      return getDefName();
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

  std::string getOperator() const override { return "=="; }
  std::string getComparisionName() const override { return "equals"; }
};

class NotEqualsFormula : public ComparisonFormula {
public:
  NotEqualsFormula(const std::shared_ptr<LeafFormula> & left, const std::shared_ptr<LeafFormula> & right)
      : ComparisonFormula(std::move(left), std::move(right))
  {
  }
  ~NotEqualsFormula() = default;

  std::string getOperator() const override { return "!="; }
  std::string getComparisionName() const override { return "not_equals"; }
};