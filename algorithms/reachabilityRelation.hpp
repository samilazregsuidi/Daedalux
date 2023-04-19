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

        unsigned long hash;
        ADD outerFeatures;
        ADD innerFeatures;
        dfs lastFoundIn;

        std::list<RState*> subStates;
    };

public:
    reachabilityRelation(void);

    virtual ~reachabilityRelation();

    void setDFS(dfs current);

    //delete filtered state
    const reachabilityRelation* filter(byte s, std::list<state*>& toFilter) const; 
   
    byte getStatus(state* s);
    
    dfs lastFoundIn(state* s) const;
    
    void update(state* s);

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


public:
    std::map<unsigned long, RState*> map; 
    dfs dfsIn;
    const TVL* tvl;
    ADD productsFailed;
};



#endif