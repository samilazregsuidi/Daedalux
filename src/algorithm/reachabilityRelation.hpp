#ifndef REACHABILITY_RELATION_H
#define REACHABILITY_RELATION_H

#include <list>
#include <memory>
#include <stack>

#include "state.hpp"
#include "tvl.hpp"

typedef char byte;
typedef unsigned char ubyte;

#include "stateVisitor.hpp"

class reachabilityRelation {

public:
  enum dfs { DFS_OUTER, DFS_INNER };

private:
  class RState {
  public:
    RState(const state * s, dfs lastFoundIn) : vId(s->getVariableId()), hash(s->hash()), lastFoundIn(lastFoundIn) {}

    /*RState(const state* s, dfs lastFoundIn, const ADD& outerFeatures, const ADD& innerFeatures)
        : vId(s->getVariableId())
        , hash(s->hash())
        , lastFoundIn(lastFoundIn)
        , outerFeatures(outerFeatures)
        , innerFeatures(innerFeatures)
    {
    }*/

    ~RState()
    {
      for (auto subS : subStates) {
        delete subS;
      }
    }

    RState * getSubHtState(const state * s)
    {
      for (auto htS : subStates) {
        if (htS->hash == s->hash() && s->getVariableId() == htS->vId)
          return htS;
      }
      return nullptr;
    }

  public:
    unsigned int vId;
    unsigned long hash;
    dfs lastFoundIn;
    ADD outerFeatures;
    ADD innerFeatures;

    std::list<RState *> subStates;
  };

  class component {
  public:
    std::string name;
    ADD productToVisit;
    ADD productFail;
    bool allProductsFail;
  };

public:
  reachabilityRelation(void);

  virtual ~reachabilityRelation();

  void init(state * init);

  void setDFS(dfs current);

  // delete filtered state
  const reachabilityRelation * filter(byte s, std::list<state *> & toFilter) const;

  byte getStatus(state * s);

  dfs lastFoundIn(state * s) const;

  void update(state * s);

  void addTraceViolation(state * loop);

  bool isComplete(void);

  bool hasErrors(void) const;

  ADD getFailedProducts(void) const;

private:
  class stateToRState : public stateVisitor {
  public:
    stateToRState(state * s, dfs dfsIn);
    operator RState *(void) const;

  private:
    void visit(state * s) override;
    void visit(process * s) override;
    void visit(program * s) override;
    void visit(composite * s) override;
    void visit(never * s) override;
    void visit(featured * s) override;

  public:
    dfs dfsIn;
    RState * res;
  };

  class getStatusVisitor : public stateVisitor {
  public:
    getStatusVisitor(RState * rstate, state * s, dfs dfsIn);

  private:
    void visit(state * s) override;
    void visit(process * s) override;
    void visit(program * s) override;
    void visit(composite * s) override;
    void visit(never * s) override;
    void visit(featured * s) override;

  public:
    RState * current;
    dfs dfsIn;
    byte res;
  };

  class updateVisitor : public stateVisitor {
  public:
    updateVisitor(reachabilityRelation * R, RState * rstate, state * s, dfs dfsIn, const TVL * tvl);

  private:
    void visit(state * s) override;
    void visit(process * s) override;
    void visit(program * s) override;
    void visit(composite * s) override;
    void visit(never * s) override;
    void visit(featured * s) override;

  public:
    RState * current;
    std::string nameComp;
    reachabilityRelation * R;
    dfs dfsIn;
  };

  class compBuilder : public stateVisitor {
  public:
    void visit(state * s) override;
    void visit(process * s) override;
    void visit(program * s) override;
    void visit(composite * s) override;
    void visit(never * s) override;
    void visit(featured * s) override;

  public:
    reachabilityRelation * R;
  };

  class violationsVisitor : public stateVisitor {
  public:
    bool isViolationsComplete(void) const;

  public:
    void visit(state * s) override;
    void visit(process * s) override;
    void visit(program * s) override;
    void visit(composite * s) override;
    void visit(never * s) override;
    void visit(featured * s) override;

  public:
    reachabilityRelation * R;
  };

public:
  std::map<unsigned long, RState *> stateMap;
  dfs dfsIn;
  const TVL * tvl;
  unsigned int nbErrors;
  std::map<std::string, component *> compMap;
};

#endif