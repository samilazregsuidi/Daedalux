#ifndef ARGUMENT_VARIABLE_H
#define ARGUMENT_VARIABLE_H

#include <vector>

struct argList {
  virtual ~argList() { for (auto a : args) delete a; }

  std::vector<argList*> args;
};

template <typename T> struct arg : public argList {
  arg(T value = 0) : value(value) {}
  T value;
};

#endif