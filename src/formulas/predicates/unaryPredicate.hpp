#pragma once

#include "../state/state.hpp"
#include "primitiveVariable.hpp"
#include "statePredicate.hpp"
#include <memory>
#include <string>

class UnaryPredicate : public statePredicate {
public:
  UnaryPredicate(std::shared_ptr<statePredicate> pred) : statePredicate(pred->getName()), pred(pred) {}

  virtual ~UnaryPredicate() = default;

  virtual bool isSatisfied(const std::shared_ptr<state> s) const = 0;

protected:
  std::shared_ptr<statePredicate> pred;
};

class NotPredicate : public UnaryPredicate {
public:
  NotPredicate(std::shared_ptr<statePredicate> pred) : UnaryPredicate(pred) {}

  virtual ~NotPredicate() = default;

  bool isSatisfied(const std::shared_ptr<state> s) const override { return !pred->isSatisfied(s); }

  bool operator==(const statePredicate & other) const override
  {
    if (const NotPredicate * p = dynamic_cast<const NotPredicate *>(&other)) {
      return *pred == *p->pred;
    }
    return false;
  }

  std::size_t hash() const override { return std::hash<std::string>{}(pred->getName()); }
};
