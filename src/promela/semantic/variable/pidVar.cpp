#include "process.hpp"

PIDVar::PIDVar(const std::string& name, process * ref) 
  : primitive<unsigned char>(name, variable::V_PID)
  ,  ref(ref)
  {
    assert(ref);
    value = ref->getPid();
  }

variable * PIDVar::deepCopy(void) const
{
  variable * copy = new PIDVar(*this);
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
  if (ref) {
    ref = getTVariable<process *>(ref->getLocalName());
    assert(ref);
  }
}