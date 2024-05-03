#ifndef ARGUMENT_VARIABLE_H
#define ARGUMENT_VARIABLE_H

#include <vector>

#include "variable.hpp"
#include "scalarVar.hpp"

struct argList {
  virtual ~argList() { for (auto a : args) delete a; }

  virtual void copyTo(variable* var) const {
    assert(args.size() == 1);
    for(auto a : args) 
      a->copyTo(var);
  }

  std::vector<argList*> args;
};

template <typename T> struct argConst : public argList {
  argConst(T value = 0) : value(value) {}
  
  void copyTo(variable* var) const override {
    auto v = dynamic_cast<scalar<T>*>(var);
    assert(v != nullptr);
    *v = value;
  }
  
  T value;

};

#endif