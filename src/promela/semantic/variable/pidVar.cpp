#include "process.hpp"

PIDVar::PIDVar(unsigned char pid) 
  : primitive<unsigned char>("", variable::V_PID, pid)
  , ref(nullptr)
{
}

PIDVar::PIDVar(const std::string& name, unsigned char pid) 
  : primitive<unsigned char>(name, variable::V_PID, pid)
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
  getPayload()->setValue<byte>(getOffset(), newRef->getPid());
}

void PIDVar::assign(const variable * sc)
{
  variable::assign(sc);
  ref = getTVariable<process *>(ref->getLocalName());
  assert(ref);
}