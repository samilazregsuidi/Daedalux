#pragma once

#include <string>
#include "CLI11.hpp"
#include "promela_loader.hpp"

#include "symbols.hpp"
#include "ast.hpp"
#include "automata.hpp"
#include "y.tab.hpp"
#include "lexer.h"
#include "tvl.hpp"
#include "ltl.hpp"

#include "semantic.hpp"
#include "ltlModelChecker.hpp"
#include "explore.hpp"


/// Collection of all options of Subcommand A.
struct ModelCheckingOptions {
	std::string input_file;
	std::string tvl_file;
	std::string ltl;
	std::string ltlPropFile;
	std::string multiLtl;
	std::string multiLtlPropFile;

	bool exhaustive;
	bool check;
	bool fullDeadlockCheck;

	bool sim;
	bool exec;

	bool printInfo;
	bool keepTempFiles;
	bool noTraces;

	unsigned int ksteps;
	unsigned int sampleSize;
	unsigned int limitExploration;
};

// Function declarations.
void setup_subcommand_modelchecking(CLI::App &app);
void run_modelchecking(ModelCheckingOptions const &opt);
bool verify_modelchecking_options(ModelCheckingOptions const &opt);