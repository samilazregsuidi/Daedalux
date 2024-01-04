#pragma once

#include <string>

#include "symbols.hpp"
#include "ast.hpp"
#include "automata.hpp"
#include "y.tab.hpp"
#include "lexer.h"
#include "astToFsm.hpp"
#include "tvl.hpp"

extern void init_lex();

class promela_loader {
    public:
        promela_loader(std::string file_name, const TVL *tvl = nullptr);

        fsm* getAutomata(void) const { return automata; }

        symTable* getSymTable(void) const { return globalSymTab; }

        stmnt* getProgram(void) const { return program; }

    private:
        fsm* automata;
        symTable* globalSymTab;
        stmnt* program;
};