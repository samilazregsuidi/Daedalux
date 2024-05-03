#ifndef SCALAR_VARIABLE_INTERFACE_H
#define SCALAR_VARIABLE_INTERFACE_H

#include "variable.hpp"
#include "payload.hpp"

#include <limits>

/**
 * @brief A templated class to represent a scalarImpl variable
*/

class scalarInt : public variable {
public:
  scalarInt(const std::string& name, Type varType)
    : variable(varType, name)
  {}

  virtual void setValue(int rvalue) = 0;

  virtual bool operator==(int) const = 0;

  virtual bool operator!=(int) const = 0;

  virtual int getValue(void) const = 0;
};

#endif