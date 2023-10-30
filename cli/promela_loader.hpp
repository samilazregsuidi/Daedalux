#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

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
        promela_loader();
        ~promela_loader(){
            delete globalSymTab;
            delete program;
        }
        
        fsm* load_promela_file(std::string file_name, const TVL *tvl = nullptr);

        symTable *get_globalSymTab() const { return globalSymTab; }
        stmnt *get_program() const { return program; }

    private:
        symTable *globalSymTab = nullptr;
        stmnt *program = nullptr;
};