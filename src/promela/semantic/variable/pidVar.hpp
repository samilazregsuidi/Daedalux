#ifndef PID_VARIABLE_H
#define PID_VARIABLE_H

#include "primitiveVar.hpp"

class process;

class PIDVar : public primitive<unsigned char> {
public:
  PIDVar(unsigned char pid = 0);

  PIDVar(const std::string& name, unsigned char initValue = 0);

  PIDVar * deepCopy(void) const override;

  process * getRefProcess(void) const;

  void setRefProcess(process * newRef);

  void assign(const variable * sc) override;

private:
  process * ref;
};

#endif