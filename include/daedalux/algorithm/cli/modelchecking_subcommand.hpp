#pragma once

#include <string>

#include <daedalux/CLI11.hpp>
#include <daedalux/promela/parser/promela_loader.hpp>

#include <daedalux/promela/symbol.hpp>
#include <daedalux/promela/ast.hpp>
#include <daedalux/core/automata.hpp>
#include <daedalux/promela/parser/promela_loader.hpp>

#include <daedalux/feature/tvl.hpp>
#include <daedalux/core/logic/ltl.hpp>

#include <daedalux/promela/semantic.hpp>
#include <daedalux/algorithm/ltlModelChecker.hpp>
#include <daedalux/algorithm/explore.hpp>



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