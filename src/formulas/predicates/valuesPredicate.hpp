#pragma once

#include "statePredicate.hpp"
#include "../formulaUtility.hpp"
#include <memory>
#include <string>

template <typename T> class LeafPredicate : public statePredicate {
public:
  LeafPredicate(std::string name) : statePredicate(name) {}

  virtual ~LeafPredicate() = default;

  virtual T getValue(const std::shared_ptr<state> s) const = 0; 

  bool isSatisfied(const std::shared_ptr<state> s) const override{
    return false;
  };

  virtual bool operator==(const statePredicate & other) const override = 0;

  std::size_t hash() const override = 0;
};

template <typename T> class ConstantPredicate : public LeafPredicate<T> {
public:
  ConstantPredicate(T value) : LeafPredicate<T>(""), value(value) {}
  virtual ~ConstantPredicate() = default;

  T getValue(const std::shared_ptr<state> s) const override { return value; };

  bool operator==(const statePredicate & other) const override {
    if (const ConstantPredicate * p = dynamic_cast<const ConstantPredicate *>(&other)) {
      return this->value == p->value;
    }
    return false;
  }

  std::size_t hash() const override { return std::hash<T>{}(value); }

private:
  T value;
};

template <typename T> class VariablePredicate : public LeafPredicate<T> {
public:
  VariablePredicate(std::string varName) : LeafPredicate<T>(varName), varName(varName) {}

  virtual ~VariablePredicate() = default;

  bool operator==(const statePredicate & other) const override {
    if (const VariablePredicate * p = dynamic_cast<const VariablePredicate *>(&other)) {
      return this->varName == p->varName;
    }
    return false;
  }

  std::size_t hash() const override { return std::hash<std::string>{}(varName); }

  T getValue(const std::shared_ptr<state> s) const
  {
    // Get all variables that are visible in the states.
    auto variables = s->getAllVisibleVariables();
    auto found_variable = std::find_if(variables.begin(), variables.end(), [&](const variable * var) {
      return var->getLocalName() == varName;
    });
    if (found_variable == variables.end()) {
      throw std::invalid_argument("Variable not found in state");
    }

    auto value = formulaUtility::getValueOfVariable(s.get(), varName);
    T correctlyTypeValue =  std::get<T>(value);
    return correctlyTypeValue;
  }

private:
  std::string varName;
};