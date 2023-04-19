#ifndef STATE_TO_GRAPH_VIZ_H
#define STATE_TO_GRAPH_VIZ_H

#include <iostream>

#include "semantic.hpp"
#include "automata.hpp"

class stateToGraphViz : public stateVisitor {
public:

    stateToGraphViz(const fsm* automata);

    ~stateToGraphViz() override;

    void printGraphViz(state* s, int depth = 0); 

    void visit(state* s) override;
	void visit(process* s) override;
	void visit(progState* s) override;
	void visit(compState* s) override;
    void visit(never* s) override;
	void visit(featStateDecorator* s) override;

private:
    std::string _tab(void) const;

public:
    const fsm* automata;
    size_t index;
    std::ofstream file;
    size_t tab;
    int depth;
    ADD featToPrint;
};

#endif