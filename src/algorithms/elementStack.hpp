#ifndef ELEMENT_STACK_H
#define ELEMENT_STACK_H

#include <stack>
#include <list>

#include "state.hpp"

class elementStack {
public:
    struct element {

        element(void);
        
        element(state* s, unsigned int depth = 0);

        ~element();

        state* s;
        std::list<state*> Post;
        bool init;
        unsigned int depth;
    };

    elementStack();
    //destructeur
    ~elementStack();

    void push(state* s, int depth = 0);

    void pop(void);
    
    element* top(void) const;
    
    bool isIn(const element& elem) const;
    
    bool isIn(unsigned long elem) const;
    
    bool empty(void) const;

    std::stack<element*> stackElem;
    std::set<unsigned long> setElem;
};

#endif