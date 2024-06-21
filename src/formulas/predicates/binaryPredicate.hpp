#pragma once

#include "statePredicate.hpp"
#include "valuesPredicate.hpp"
#include <string>

template <typename T> class BinaryPredicate : public statePredicate {
public:
  BinaryPredicate(std::string & name, std::shared_ptr<LeafPredicate<T>> left, std::shared_ptr<LeafPredicate<T>> right)
      : statePredicate(name), left(left), right(right)
  {
  }

  virtual ~BinaryPredicate() = default;

  virtual bool isSatisfied(const std::shared_ptr<state> s) const = 0;

  std::shared_ptr<LeafPredicate<T>> getLeft() const { return left; }

  std::shared_ptr<LeafPredicate<T>> getRight() const { return right; }

  virtual bool operator==(const statePredicate & other) const = 0;

  std::size_t hash() const override = 0;

protected:
  std::shared_ptr<LeafPredicate<T>> left;
  std::shared_ptr<LeafPredicate<T>> right;
};

template <typename T> class EqualityPredicate : public BinaryPredicate<T> {
public:
  EqualityPredicate(std::string & name, std::shared_ptr<LeafPredicate<T>> left, std::shared_ptr<LeafPredicate<T>> right)
      : BinaryPredicate<T>(name, left, right)
  {
  }

  virtual ~EqualityPredicate() = default;

  bool isSatisfied(const std::shared_ptr<state> s) const override
  {
    auto left_child = this->getLeft(); // Explicitly specify this-> to access the inherited method
    auto right_child = this->getRight();
    auto left_val = left_child->getValue(s);
    auto right_val = right_child->getValue(s);
    return left_val == right_val;
  }

  bool operator==(const statePredicate & other) const override
  {
    if (const EqualityPredicate * p = dynamic_cast<const EqualityPredicate *>(&other)) {
      return *this->left == *p->left && *this->right == *p->right;
    }
    return false;
  }

  std::size_t hash() const override
  {
    return std::hash<std::string>{}(this->left->getName()) ^ std::hash<std::string>{}(this->right->getName());
  }
};

template <typename T> class InequalityPredicate : public BinaryPredicate<T> {
public:
  InequalityPredicate(std::string & name, std::shared_ptr<LeafPredicate<T>> left, std::shared_ptr<LeafPredicate<T>> right)
      : BinaryPredicate<T>(name, left, right)
  {
  }

  virtual ~InequalityPredicate() = default;

  bool isSatisfied(const std::shared_ptr<state> s) const override
  {
    auto left_child = this->getLeft(); // Explicitly specify this-> to access the inherited method
    auto right_child = this->getRight();
    auto left_val = left_child->getValue(s);
    auto right_val = right_child->getValue(s);
    return left_val != right_val;
  }

  bool operator==(const statePredicate & other) const override
  {
    if (const InequalityPredicate * p = dynamic_cast<const InequalityPredicate *>(&other)) {
      return *this->left == *p->left && *this->right == *p->right;
    }
    return false;
  }

  std::size_t hash() const override
  {
    return std::hash<std::string>{}(this->left->getName()) ^ std::hash<std::string>{}(this->right->getName());
  }
};

template <typename T> class LessThanPredicate : public BinaryPredicate<T> {
public:
  LessThanPredicate(std::string & name, std::shared_ptr<LeafPredicate<T>> left, std::shared_ptr<LeafPredicate<T>> right)
      : BinaryPredicate<T>(name, left, right)
  {
  }

  virtual ~LessThanPredicate() = default;

  bool isSatisfied(const std::shared_ptr<state> s) const override
  {
    auto left_child = this->getLeft(); // Explicitly specify this-> to access the inherited method
    auto right_child = this->getRight();
    auto left_val = left_child->getValue(s);
    auto right_val = right_child->getValue(s);
    return left_val < right_val;
  }

  bool operator==(const statePredicate & other) const override
  {
    if (const LessThanPredicate * p = dynamic_cast<const LessThanPredicate *>(&other)) {
      return *this->left == *p->left && *this->right == *p->right;
    }
    return false;
  }

  std::size_t hash() const override
  {
    return std::hash<std::string>{}(this->left->getName()) ^ std::hash<std::string>{}(this->right->getName());
  }
};

template <typename T> class LessThanOrEqualPredicate : public BinaryPredicate<T> {
public:
  LessThanOrEqualPredicate(std::string & name, std::shared_ptr<LeafPredicate<T>> left, std::shared_ptr<LeafPredicate<T>> right)
      : BinaryPredicate<T>(name, left, right)
  {
  }

  virtual ~LessThanOrEqualPredicate() = default;

  bool isSatisfied(const std::shared_ptr<state> s) const override
  {
    auto left_child = this->getLeft(); // Explicitly specify this-> to access the inherited method
    auto right_child = this->getRight();
    auto left_val = left_child->getValue(s);
    auto right_val = right_child->getValue(s);
    return left_val <= right_val;
  }

  bool operator==(const statePredicate & other) const override
  {
    if (const LessThanOrEqualPredicate * p = dynamic_cast<const LessThanOrEqualPredicate *>(&other)) {
      return *this->left == *p->left && *this->right == *p->right;
    }
    return false;
  }

  std::size_t hash() const override
  {
    return std::hash<std::string>{}(this->left->getName()) ^ std::hash<std::string>{}(this->right->getName());
  }
};