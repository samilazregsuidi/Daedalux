#ifndef PROCESS_STATE_H
#define PROCESS_STATE_H

#include <cassert>
#include <list>
#include <map>
#include <tuple>

#include "thread.hpp"

typedef char byte;

#include "astNode.hpp"
#include "automata.hpp"
#include "programState.hpp"
#include "state.hpp"

// A state mask gives for every process the pid, a pointer to its symtab node
// and its offset in the payload
class process : public thread {
public:
  friend class state;

  process(const seqSymNode * sym, const fsmNode * start, byte pid, unsigned int index = 0);

  process(const process & other);

  process(const process * other);

  ~process() override;

  process * deepCopy(void) const override;

  void init(void) override;

  byte getPid(void) const;

  void setPid(byte pid);

  // operator std::string(void) const override;

  void print(void) const override;

  void printCSVHeader(std::ostream & out) const override;

  void printCSV(std::ostream & out) const override;

  std::list<transition *> transitions(void) const override;

  void setProgState(progState * newS);

  progState * getProgState(void) const;

  std::list<transition *> executables(void) const override;

  void apply(transition * trans) override;

// Expression evaluation (flag)
#define EVAL_EXECUTABILITY 0
#define EVAL_EXPRESSION    1

  int eval(const astNode * exp, byte flag) const override;

  int eval(const fsmEdge * edge, byte flag) const override;

  bool isAccepting(void) const override;

  state * getNeverClaim(void) const override;

  bool safetyPropertyViolation(void) const override;

  void accept(stateVisitor * visitor) override;

public:
  byte pid;
};

#endif