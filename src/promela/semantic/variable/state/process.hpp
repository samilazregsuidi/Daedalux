#ifndef PROCESS_STATE_H
#define PROCESS_STATE_H

#include <cassert>
#include <list>
#include <map>
#include <tuple>

#include "thread.hpp"

#include "astNode.hpp"
#include "automata.hpp"
#include "program.hpp"
#include "state.hpp"

// A state mask gives for every process the pid, a pointer to its symtab node
// and its offset in the payload
class process : public thread {
public:
  friend class state;

  process(const std::string& name, const fsmNode * start);

  process(const process & other);

  ~process() override;

  process * deepCopy(void) const override;

  // operator std::string(void) const override;

  void print(void) const override;

  void printCSVHeader(std::ostream & out) const override;

  void printCSV(std::ostream & out) const override;

  std::list<transition *> transitions(void) const override;

  void setProgState(program * newS);

  program * getProgState(void) const;

  channel* getChannel(const std::string& name) const;

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
};

#endif