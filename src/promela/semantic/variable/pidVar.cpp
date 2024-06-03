/*#include "pidVar.hpp"

#include "process.hpp"

PIDVar::PIDVar(unsigned char pid) 
  : scalar<unsigned char, V_PID>("", pid)
  , ref(nullptr)
{
}

PIDVar::PIDVar(const std::string& name, unsigned char pid) 
  : scalar<unsigned char, V_PID>(name, pid)
  , ref(nullptr)
{
}

PIDVar * PIDVar::deepCopy(void) const
{
  PIDVar * copy = new PIDVar(*this);
  return copy;
}

process * PIDVar::getRefProcess(void) const { return ref; }

void PIDVar::setRefProcess(process * newRef)
{
  ref = newRef;

  scalar<unsigned char, V_PID>::setValue(newRef->getPid());
}

void PIDVar::assign(const variable * sc)
{
  variable::assign(sc);
  ref = get<process *>(ref->getLocalName());
  assert(ref);
}*/