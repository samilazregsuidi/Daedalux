#ifndef REACHABILITY_RELATION_H
#define REACHABILITY_RELATION_H

#include <stack>
#include <list>

#include "state.hpp"
#include "tvl.hpp"

typedef char byte;
typedef unsigned char ubyte;

#include "stateVisitor.hpp"

class reachabilityRelation {

public:

    enum status {
        S_NEW,
        S_VISITED,
        S_FRESH
    };

    enum dfs {
        DFS_OUTER,
        DFS_INNER
    };

private:
    class RState {
    public:
        RState(unsigned long hash, dfs lastFoundIn)
            : hash(hash)
            , lastFoundIn(lastFoundIn)
        {
        }
        
        RState(unsigned long hash, dfs lastFoundIn, const ADD& outerFeatures, const ADD& innerFeatures) 
            : hash(hash)
            , outerFeatures(outerFeatures)
            , innerFeatures(innerFeatures)
            , lastFoundIn(lastFoundIn)
        {
        }

        ~RState() {
            for(auto subS : subStates) {
                delete subS;
            }
        }

        RState* getSubHtState(unsigned long hash) {
            for(auto htS : subStates)  {
                if(htS->hash == hash)
                    return htS;
            }
            return nullptr;
        }

    public:
        unsigned long hash;
        ADD outerFeatures;
        ADD innerFeatures;
        dfs lastFoundIn;

        std::list<RState*> subStates;
    };

    class component {
    public:
        std::string name;
        ADD productToVisit;
        bool allProductsFail;
    };

public:
    reachabilityRelation(void);

    virtual ~reachabilityRelation();

    void init(state* init);

    void setDFS(dfs current);

    //delete filtered state
    const reachabilityRelation* filter(byte s, std::list<state*>& toFilter) const; 
   
    byte getStatus(state* s);
    
    dfs lastFoundIn(state* s) const;
    
    void update(state* s);

    void addTraceViolation(state* loop);

    bool isComplete(void) const;

    bool hasErrors(void) const;

private:
    class stateToRState : public stateVisitor {
    public:
        stateToRState(state* s, dfs dfsIn);
        operator RState*(void) const;

    private:
        void visit(state* s) override;
        void visit(process* s) override;
        void visit(progState* s) override;
        void visit(compState* s) override;
        void visit(never* s) override;
        void visit(featStateDecorator* s) override;
   
    public:
        dfs dfsIn;
        RState* res;
    };

    class getStatusVisitor : public stateVisitor {
    public:
        getStatusVisitor(RState* rstate, state* s, dfs dfsIn);

    private:
        void visit(state* s) override;
        void visit(process* s) override;
        void visit(progState* s) override;
        void visit(compState* s) override;
        void visit(never* s) override;
        void visit(featStateDecorator* s) override;
    
    public:
        RState* current;
        dfs dfsIn;
        byte res;
    };

    class updateVisitor : public stateVisitor {
    public:
        updateVisitor(RState* rstate, state* s, dfs dfsIn, const TVL* tvl);

    private:
        void visit(state* s) override;
        void visit(process* s) override;
        void visit(progState* s) override;
        void visit(compState* s) override;
        void visit(never* s) override;
        void visit(featStateDecorator* s) override;
    
    public:
        RState* current;
        dfs dfsIn;

        const TVL* tvl;
    };

    class compBuilder : public stateVisitor {
    public:
        void visit(state* s) override;
        void visit(process* s) override;
        void visit(progState* s) override;
        void visit(compState* s) override;
        void visit(never* s) override;
        void visit(featStateDecorator* s) override;
    public:
        std::map<std::string, component> compMap;
    };

    class violationsVisitor : public stateVisitor {
    public:
        bool isViolationsComplete(void) const;

    public:
        void visit(state* s) override;
        void visit(process* s) override;
        void visit(progState* s) override;
        void visit(compState* s) override;
        void visit(never* s) override;
        void visit(featStateDecorator* s) override;
    
    public:
        std::map<std::string, component> compMap;
    };




public:
    std::map<unsigned long, RState*> map; 
    dfs dfsIn;
    const TVL* tvl;
    violationsVisitor violations;
    unsigned int nbErrors;
};



#endif