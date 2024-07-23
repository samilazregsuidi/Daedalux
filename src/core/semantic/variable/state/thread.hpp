#ifndef THREAD_STATE_H
#define THREAD_STATE_H

#include <cassert>
#include <list>
#include <map>
#include <tuple>

#include "state.hpp"
#include "transition.hpp"
#include "paramList.hpp"

#include "symbols.hpp"

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

  thread(variable::Type type, const std::string& name, const fsmNode* start, ubyte pid);

  thread(const thread & other);

  void init(void) override;

  ubyte getPid(void) const;

  void setPid(ubyte pid);

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

  //variable * getVariableFromExpr(const expr * varExpr) const;

  //argList getArgs(const exprArgList * args) const;

  //std::list<const arg> getConstArgs(const exprArgList * args) const;

  paramList getOutputParamList(const exprRArgList * rargs) const;

  paramList getInputParamList(const exprArgList * rargs) const;

  //std::list<const arg> getConstRArgs(const exprRArgList * rargs) const;

  //channel * getChannelFromExpr(const expr * varExpr) const;

  bool operator==(const variable * other) const override;

  bool operator!=(const variable * other) const override;

  variable * operator=(const variable * other) override;

  /*template <typename T> T* getTVar(const expr* varExpr, const thread* proc) const {
          return dynamic_cast<T*>(getVariable(varExpr));
  }*/

  void printGraphViz(unsigned long i) const;

  // byte compare(const state& s2) const override;

  std::vector<std::shared_ptr<statePredicate>> getPredicates(void) const override;

  float delta(const variable * v2, bool considerInternalVariables) const override;

  std::list<variable *> getDelta(const variable * v2, bool considerInternalVariables) const override;

  void printDelta(const variable * v2, bool considerInternalVariables) const override;

  size_t size(void) const override;

  unsigned long hash(void) const override;

protected:

  void hash(byte* payload) const override;

public:
  const fsmNode * const start;
  const fsmNode * loc;
  mutable bool _else;
  ubyte pid;
};

#endif