#ifndef ARGUMENT_VARIABLE_H
#define ARGUMENT_VARIABLE_H

#include <vector>

#include "variable.hpp"
#include "scalarVar.hpp"

struct param {

  enum Type {
    VAL,
    REF
  };

  virtual void in(variable* var) const = 0;
  virtual bool out(const variable* var) = 0;
  virtual int getIntValue() const = 0;

  Type type;
};

struct paramValue : public param {
  paramValue(int value) : value(value) {
    type = Type::VAL;
  }

  void in(variable* var) const override {
    auto v = dynamic_cast<scalarInt*>(var);
    assert(v != nullptr);
    v->setIntValue(value);
  }

  bool out(const variable* var) override {
    auto v = dynamic_cast<const scalarInt*>(var);
    assert(v != nullptr);
    return (v->getIntValue() == value);
  }

  int getIntValue() const override {
    return value;
  }
  
  int value;
};

struct paramRef : public param {
  paramRef(scalarInt* ref) : ref(ref) {
    type = Type::REF;
  }
  
  void in(variable* var) const override {
    auto v = dynamic_cast<scalarInt*>(var);
    assert(v != nullptr);
    v->setIntValue(ref->getIntValue());
  }

  bool out(const variable* var) override {
    auto v = dynamic_cast<const scalarInt*>(var);
    assert(v != nullptr);
    ref->setIntValue(v->getIntValue());
    return true;
  }

  int getIntValue() const override {
    return ref->getIntValue();
  }
  
  scalarInt* ref;
};

struct paramList : public std::vector<param*> {

  virtual void in(variable* var) const {
    auto vars = var->getVariablesVector();
    assert(size() == vars.size());
    for(size_t i = 0; i < vars.size(); ++i) { 
      (*this)[i]->in(vars[i]);
    }
  }

  virtual void out(const variable* var) {
    auto vars = var->getVariablesVector();
    assert(size() == vars.size());
    for(size_t i = 0; i < vars.size(); ++i) { 
      (*this)[i]->out(vars[i]);
    }
  }
};

#endif