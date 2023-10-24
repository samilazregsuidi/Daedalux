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

#include "astToFsm.hpp"
#include "semantic.hpp"
#include "explore.hpp"

/// Collection of all options of Subcommand A.
struct ModelCheckingOptions {
	std::string input_file;
	std::string tvl_file;
	std::string propFile;
	std::string tvlFilter;

	bool exhaustive;
	bool check;
	bool fullDeadlockCheck;

	bool sim;
	bool enum_;
	bool exec;

	bool bfs;
	bool stutterSteps;

	bool printInfo;
	bool keepTempFiles;
	bool noTraces;

	bool spinMode;
	bool optimisedSpinMode;

	unsigned int ksteps;
	unsigned int bound;
	unsigned int sampleSize;
	unsigned int limitExploration;

	std::pair<std::string, std::string> tvl_files = std::make_pair("", "");
};

// Function declarations.
void setup_subcommand_modelchecking(CLI::App &app);
void run_modelchecking(ModelCheckingOptions const &opt);
bool verify_modelchecking_options(ModelCheckingOptions const &opt);