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

extern void init_lex();

void load_promela_file(std::string file_name, symTable *&globalSymTab, stmnt *&program);