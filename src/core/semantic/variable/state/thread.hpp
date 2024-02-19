#ifndef THREAD_STATE_H
#define THREAD_STATE_H

#include <cassert>
#include <list>
#include <map>
#include <tuple>

#include "state.hpp"
#include "transition.hpp"

typedef char byte;

#include "symbols.hpp"

#include "channel.hpp"
#include "variable.hpp"

#include "astNode.hpp"
#include "expr.hpp"

#include "automata.hpp"

class exprArgList;
class exprRArgList;

// A state mask gives for every process the pid, a pointer to its symtab node
// and its offset in the payload
class thread : public state {
public:
  friend class state;

  thread(variable::Type type, const seqSymNode * sym, const fsmNode * start, unsigned int index = 0);

  thread(const thread & other);

  thread(const thread * other);

  void init(void) override;

  byte getPid(void) const;

  void setPid(byte pid);

  // virtual std::list<transition*> transitions(void) const = 0;

  const fsmNode * getFsmNodePointer(void) const;

  void setFsmNodePointer(const fsmNode * pointer);

  int getLocation(void) const;

  bool isAtLabel(int nbLine) const;

  bool nullstate(void) const override;

  bool endstate(void) const override;

// Expression evaluation (flag)
#define EVAL_EXECUTABILITY 0
#define EVAL_EXPRESSION    1

  virtual int eval(const astNode * node, byte flag) const = 0;

  // trans or state, signature can be optimized!
  virtual int eval(const fsmEdge * edge,
                   byte flag) const = 0; // Return true <=> transition 'trans' is executable on process 'mask'.

  std::string getName(void) const;

  bool isAccepting(void) const override;

  bool isAtomic(void) const;

  std::string getVarName(const expr * varExpr) const;

  variable * getVariable(const expr * varExpr) const;

  std::list<variable *> getVariables(const exprArgList * args) const;

  std::list<const variable *> getConstVariables(const exprArgList * args) const;

  std::list<variable *> getVariables(const exprRArgList * rargs) const;

  std::list<const variable *> getConstVariables(const exprRArgList * rargs) const;

  channel * getChannel(const expr * varExpr) const;

  bool operator==(const variable * other) const override;

  bool operator!=(const variable * other) const override;

  /*template <typename T> T* getTVar(const expr* varExpr, const thread* proc) const {
          return dynamic_cast<T*>(getVariable(varExpr));
  }*/

  void printGraphViz(unsigned long i) const;

  // byte compare(const state& s2) const override;

  float delta(const variable * v2) const override;

  float deltaProcess(const thread * s2) const;

  std::list<variable *> getDelta(const variable * v2) const override;

  void printDelta(const variable * v2) const override;

public:
  const seqSymNode * symType;
  unsigned int index;

  const fsmNode * const start;

  mutable bool _else;
};

#endif