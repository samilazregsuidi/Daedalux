#ifndef ELEMENT_STACK_H
#define ELEMENT_STACK_H

#include <stack>
#include <list>
#include <memory>

#include "state.hpp"

class elementStack {
public:
    struct element {

        element(void);
        
        element(std::shared_ptr<state> s, unsigned int depth = 0);

        ~element() = default;

        std::shared_ptr<state> current_state;
        std::list<std::shared_ptr<state>> Post;
        bool init;
        unsigned int depth;
    };

    elementStack() = default; //constructors
    
    ~elementStack() = default; //destructors

    void push(std::shared_ptr<state> s, int depth = 0);

    void pop(void);

    void pop(unsigned int numberOfElements);
    
    std::shared_ptr<element> top(void) const;
    
    bool isIn(const element& elem) const;
    
    bool isIn(unsigned long elem) const;
    
    bool empty(void) const;

    std::stack<std::shared_ptr<element>> stackElem;
    std::set<unsigned long> setElem;
};

#endif