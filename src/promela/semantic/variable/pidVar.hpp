#ifndef PID_VARIABLE_H
#define PID_VARIABLE_H

#include "primitiveVar.hpp"

class process;

class PIDVar : public primitive<unsigned char> {
public:
  PIDVar(const std::string& name, process* ref);

  variable * deepCopy(void) const override;

  process * getRefProcess(void) const;

  void setRefProcess(process * newRef);

  void assign(const variable * sc) override;

private:
  process * ref;
};

#endif