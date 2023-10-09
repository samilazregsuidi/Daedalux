#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <algorithm>
#include <filesystem>
#include <CLI/CLI.hpp>

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

extern void init_lex();

// Settings defined in main

// Other global variables from main
symTable* globalSymTab = nullptr;
stmnt* program = nullptr;
TVL* tvl = nullptr;

/**
 * Simply copies a file byte by byte; could be made more efficient.
 */
int copyFile(const std::string& source, const std::string& target) {
	if(std::filesystem::copy_file(source, target) != 0) return 0;
	return 1;
}


#define NB_LASSO 1
#define FULL_TRACE 2
#define SIMPLE_TRACE 1
#define NO_TRACE 0

// Settings defined in main
byte trace = FULL_TRACE;
byte keepTempFiles = 0;
byte spinMode = 0;
byte enum_ = 0;
byte optimisedSpinMode = 0;
byte exhaustive = 0;
byte fullDeadlockCheck = 0;
byte sim = 0;
byte stutterStep = 0;
bool guided = 0;
unsigned int sampling = 0;
unsigned int ksteps = 0;
byte bfs = 0;
long int limitExploration;
unsigned int bound = 9999999;


// Profiler variables
struct timeval _profileTimeBegin, _profileTimeEnd;
char* _profilerStr = nullptr;


/**
 * To be used for command-line parameter errors.  Does the
 * same as failure(), and prints the usage instructions.
 */
void error(const char* msg, ...) {
	printf("Error: ");
	va_list args;
	va_start(args, msg);
	vfprintf(stdout, msg, args);
	va_end(args);
	printf(	"\n --\n"
			"Usage: ./provelines [options] model.pml\n"
			"\n"
			"  (none)   Does nothing.\n"
			" Options for output control:\n"
			"  (none)   Prints a full trace for every counterexample.\n"
			" Options for features and feature model:\n"
			"  (none)   Will attempt to load a TVL feature model that is named as the\n"
			"           .pml file but with extension .tvl\n"
			"  -fmdimacs <dimacsClauseFile.txt> <mappingFile.txt>\n"
			"           As before, but load the dimacs of the feature model directly.\n"
			" --\n");
	exit(-1);
}

int main_cli(int argc, char *argv[]) {

	// Some basic validity checks
	if(sizeof(int)   != 4)			{ std::cout << "Bad architecture: int type must be four bytes long.\n"; exit(1); }
	if(sizeof(short) != 2)			{ std::cout << "Bad architecture: short type must be two bytes long.\n"; exit(1); }
	if(sizeof(unsigned long) != 8)  { std::cout << "Bad architecture: long type must be two bytes long.\n"; exit(1); }
	if(sizeof(double) != 8)  		{ std::cout << "Bad architecture: double type must be two bytes long.\n"; exit(1); }
	if(sizeof(void*) != 8)  		{ std::cout << "Bad architecture: pointer type must be eight bytes long.\n"; exit(1); }

	CLI::App app;

	std::string pmlFile;
	app.add_option("-m,--model", pmlFile, "Model to verify")
	->required()
	->check(CLI::ExistingFile);

	// Options for model checking
	app.add_flag("-exhaustive", exhaustive, "Determines also which products have *no* problems. The normal check will stop at the first problem,  and does not determine whether there are products that have no problems  (e.g. those that satisfy a property).");

	bool check = false;
	app.add_flag("-check", check, "Verifies the model.  When a problem (assert, deadlock or property violation) is found, an error trace (counterexample) is printed and execution stops.");

	app.add_flag("-bfs", bfs, "Performs a breadth-first search instead of a depth-first search.");

	app.add_option("-ksteps", ksteps, "Bounded sampling to sample of lenght k.")->check(CLI::PositiveNumber);

	app.add_option("-sampling", sampling, "Sampling to sample of lenght k.")->check(CLI::PositiveNumber);

	app.add_flag("-fdlc", fullDeadlockCheck, "Search for trivially invalid end states (more costly)");

	app.add_flag("-stutter", stutterStep, "Performs a stutter step search.");

	// Output control
	bool st = false;
	app.add_flag("-st", st, "Only prints states when there are no changed variables.");

	bool nt = false;
	app.add_flag("-nt", nt, "Does not print any traces, only information  about  violating (or satisfying) products.");

	// Options for features and feature model
	std::string tvlFile;
	app.add_option("-fm", tvlFile, 
	"Load the specified TVL file (only used in verification). This parameter can be omitted if the TVL file is named as the .pml file but with extension .tvl.")
	->check(CLI::ExistingFile);

	app.add_option("-fmdimacs", 
	"Load the specified TVL file (only used in verification). This parameter can be omitted if the TVL file is named as the .pml file but with extension .tvl.")
	->check(CLI::ExistingFile);

	app.add_flag("-enum", enum_, "Iterate over every product of the product line.");

	std::string tvlFilter;
	app.add_option("-filter", tvlFilter, "Limit the verification to a subset of the products  specified in the TVL file.  The TVL syntax has to be used for this.");

	app.add_flag("-spin", spinMode, "Treat features like normal variables (as SPIN would do).");

	app.add_flag("-ospin", optimisedSpinMode, "Similar to -spin, but statements with a bad guard are removed from the model before model checking.  The input is thus interpreted as fPromela (not exactly as SPIN would do). This is normally more efficient than -spin.");

	// Options for debugging

	bool exec = false;
	app.add_flag("-exec", exec, "Execute the model (does not print states, only model output).");

	unsigned int limitExploration = 0;
	app.add_option("-l", limitExploration, "Stop when the given number of states were explored. (This option can also be used for model checking.)")
	->check(CLI::PositiveNumber);

	bool printStaticInfo = false;
	app.add_flag("-s", printStaticInfo, "Prints static information about the model (symbol table, FSMs, MTypes, ..).");
	app.add_flag("-t", keepTempFiles, "Do not delete the generated temporary files.");

	bool no_trace = false;
	app.add_flag("-nt", no_trace, "Do not print any traces, only information about violating (or satisfying) products.");

	bool simple_trace = false;
	app.add_flag("-st", simple_trace, "Only prints states when there are no changed variables.");

	std::string propFile;
	app.add_option("-props", propFile, "File containing the properties to check.")
	->check(CLI::ExistingFile);



	CLI11_PARSE(app, argc, argv);

	// The variable promelaFile should have the fileExtension .pml
	if(pmlFile.find(".pml") == std::string::npos) error("The model file must have the extension .pml.");

	// The variable tvlFile should have the fileExtension .tvl
	if(tvlFile.find(".tvl") == std::string::npos) error("The feature model file must have the extension .tvl.");

	// Copy the model file to a temporary file
	if(!copyFile(pmlFile, "__workingfile.tmp")) { std::cout << "The fPromela file does not exist or is not readable!\n"; exit(1); }


	if(spinMode && optimisedSpinMode) error("The options -spin and -ospin cannot be used together.");
	if(sampling > 0 && ksteps > 0) error("The options -sampling and -ksteps cannot be used together.");
	if(check && exec) error("The options -check and -exec cannot be used together.");
	if(check && enum_) error("The options -check and -enum cannot be used together.");


	tvl = new TVL();

	//tvl->loadFeatureModelDimacs(argv[i+1], argv[i+2]);



	
	/**********************************************************************************************************************************/

	std::string path;
	std::string ltlProp;

	int i, fm = 0, ltl = 0;


	std::string argv_0 = std::string(argv[0]);
	path = argv_0.substr(0 , (argv_0.size() + 1) - sizeof("deadalux"));
	
	std::cout << path << std::endl;
	
	// Read command-line arguments
	for(int i = 1; i < argc-1; i++) {
		if(strcmp(argv[i],"-guided") == 0) {
			guided = 1;
		}else if(strcmp(argv[i],"-fm") == 0) {
			if(fm) error("The options -fm and -fmdimacs cannot be used at the same time.");
			fm = 1;
			i++;
			if(i >= argc - 1) error("The -fm option has to be followed by the feature model file name.");
			else tvlFile = argv[i];

		} else if(strcmp(argv[i],"-fmdimacs") == 0) {
			if(fm) error("The options -fm and -fmdimacs cannot be used at the same time.");
			fm = 1;
			if(i+2 >= argc - 1) error("The -fmdimacs option has to be followed by the dimacs clause file and the dimacs mapping file.");
			else tvl->loadFeatureModelDimacs(argv[i+1], argv[i+2]);
			i += 2;
		} else if(strcmp(argv[i],"-ltl") == 0) {
			ltl = 1;
			i++;
			if(i >= argc - 1) error("The -ltl option has to be followed by an LTL property.");
			else {
				ltlProp = argv[i];
				std::string errorMsg;
				if(!appendClaim("__workingfile.tmp", path, ltlProp, errorMsg)) error("The LTL formula '%s' could not be parsed, error message: \n --\n%s\n", ltlProp, errorMsg);
			}


		} else if(strcmp(argv[i], "-props") == 0) {
			i++;
			propFile = argv[i];
		}else if(strcmp(argv[i], "-bound") == 0) {
			i++;
			if(i >= argc - 1) error("The -ltl option has to be followed by an LTL property.");
			bound = atoi(argv[i]);
		}
		else {
			error("Unrecognised option '%s'", argv[i]);
		}
	}

	/**********************************************************************************************************************************/

	// Some basic validity checks

	if(fm && tvlFile.empty() && !tvlFilter.empty()) error("The -filter option cannot be used with the -fmdimacs option, only with -fm.");
	if(fm && !tvlFile.empty()) {
		if(!tvl->loadFeatureModel(tvlFile, tvlFilter)) error("Could not load the specified feature model file.");
	} else if(!fm && /*!spinM* &&*/ !optimisedSpinMode) {
		// Try to guess name of feature model file name
		std::string tvlFile = std::string(argv[argc - 1]).replace(pmlFile.find(".pml"), 4, ".tvl");
		printf("tvl file = %s\n", tvlFile.c_str());
		
		if(!tvl->loadFeatureModel(tvlFile, tvlFilter) && !tvlFilter.empty()) error("The -filter option can only be used when a feature model is charged.");
	} else {
		if(!fm && (spinMode || optimisedSpinMode) && !tvlFilter.empty()) error("The -filter option can only be used when a feature model is charged.");
	}

	if(propFile.empty() && (sim || stutterStep)) {
		error("Simulation checking and non stutter steps require a property file.");
	}

	// Invoke cpp
	if(system("cpp __workingfile.tmp __workingfile.tmp.cpp") != 0) { std::cout << "Could not run the c preprocessor (cpp).\n"; exit(1); }

	yyin = fopen("__workingfile.tmp.cpp", "r");
	if(yyin == nullptr) { std::cout << "Could not open temporary working file ("<<argv[argc - 1]<<").\n"; exit(1); }
	init_lex();

	if(yyparse(&globalSymTab, &program) != 0) { 
		std::cout << "Syntax error; aborting..\n"; exit(1); 
	}

	if(yyin != nullptr) {
		fclose(yyin);
		yylex_destroy();
	}

	// Initialize srand
	srand(time(nullptr));

	ASTtoFSM* converter = new ASTtoFSM();
	// Create the automata from the AST
	fsm* automata = converter->astToFsm(globalSymTab, program, tvl);
	automata->orderAcceptingTransitions();


	int sum = 0;
	int index = 0;
	for(; index < 1; index++) {
		sum += launchExecutionMarkovChain(automata, tvl);
	}
	std::cout << sum << std::endl; 
	double avg = (float)sum / index;
	std::cout << "proportion to satisfy the bltl property : " << avg << std::endl;

	/*for(int i = 0; i < NB_LASSO; ++i)
		findLasso(automata, K);*/

	//createStateSpaceBFS(automata, tvl);

	//ltlModelChecker* mc = new ltlModelChecker();
	//mc->startNestedDFS(automata, tvl);
	//delete mc;

	//createStateSpaceDFS_RR(automata, tvl);

	//createStateSpaceDFS(automata, tvl);

	std::ofstream symtable;
	symtable.open("sym_table_graphviz.dot");
	
	while(globalSymTab->prevSymTab()) 
		globalSymTab = globalSymTab->prevSymTab();

	globalSymTab->printGraphViz(symtable);
	symtable.close();

	//output.open("mutants/original_.pml");
	//output << stmnt::string(program);
	//output.close();

	
	//state* init = new state(globalSymTab, automata);

	//Clean up memory

	if(converter)
		delete converter;

	if(automata)
		delete automata;

	if(tvl)
		delete tvl;

	TVL::deleteBoolFct();

	delete globalSymTab;

	delete program;
	
	exit(0);
}
