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
int sampling = 0;
int ksteps = 0;
byte bfs = 0;
long int limitExploration;
int bound = 9999999;


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
			"Usage: ./daedalux [options] model.pml\n"
			"\n"
			" Options for model checking:\n"
			"  (none)   Does nothing.\n"
			"  -check   Verifies the model.  When a problem (assert, deadlock or pro-\n"
			"           perty violation) is found, an error trace (counterexample) is\n"
			"           printed and execution stops. \n"
			"  -exhaustive \n"
			"           Determines also which products have *no* problems. The normal\n"
			"           check will stop at the first problem,  and does not determine\n"
			"           whether there are products that have no problems  (e.g. those\n"
			"           that satisfy a property).\n"
			"  -sampling    Search  by executions sampling.\n"
			"  -ksteps	Bounded sampling to sample of lenght k.\n"
			"  -bfs \n"
			"           Performs a breadth-first search instead of a depth-first search.\n"
			"  -fdlc    Search for trivially invalid end states (more costly).\n"
			" Options for output control:\n"
			"  (none)   Prints a full trace for every counterexample.\n"
			"  -st      Only prints states when there are no changed variables.\n"
			"  -nt      Does not print any traces, only information  about  violating\n"
			"           (or satisfying) products.\n"
			" Options for features and feature model:\n"
			"  (none)   Will attempt to load a TVL feature model that is named as the\n"
			"           .pml file but with extension .tvl\n"
			"  -fm <file.tvl> \n"
			"           Load the specified TVL file (only used in verification). This\n"
			"           parameter can be omitted if the TVL file is named as the .pml\n"
			"           file but with extension .tvl.\n" 
			"  -fmdimacs <dimacsClauseFile.txt> <mappingFile.txt>\n"
			"           As before, but load the dimacs of the feature model directly.\n"
			"  -filter <expression> \n"
			"           Limit the verification to a subset of the products  specified\n"
			"           in the TVL file.  The TVL syntax has to be used for this.\n"
			"  -spin    Treat features like normal variables (as SPIN would do).\n"
			"  -enum    Iterate over every product of the product line.\n"
			"  -ospin   Similar to -spin, but statements with a bad guard are removed\n"
			"           from the  model before model checking.  The input is thus in-\n"
			"           terpreted as fPromela (not exactly as SPIN would do). This is\n"
			"           normally more efficient than -spin.\n"
			" Options for debugging:\n"
			"  -exec    Execute the model (does not print states, only model output).\n"
			"  -l <integer> \n"
			"           Stop when the given number of states were explored. (This op-\n"
			"           tion can also be used for model checking.)\n"
			"  -s       Parse and print static info (symbol table, FSMs, MTypes, ..).\n"
			"  -t       Do not delete the generated temporary files.\n"
			"\n"
			" --\n");
	exit(-1);
}

int main(int argc, char *argv[]) {

	if(sizeof(int)   != 4)			{ std::cout << "Bad architecture: int type must be four bytes long.\n"; exit(1); }
	if(sizeof(short) != 2)			{ std::cout << "Bad architecture: short type must be two bytes long.\n"; exit(1); }
	if(sizeof(unsigned long) != 8)  { std::cout << "Bad architecture: long type must be two bytes long.\n"; exit(1); }
	if(sizeof(double) != 8)  		{ std::cout << "Bad architecture: double type must be two bytes long.\n"; exit(1); }
	if(sizeof(void*) != 8)  		{ std::cout << "Bad architecture: pointer type must be eight bytes long.\n"; exit(1); }

	
	/**********************************************************************************************************************************/

	if(argc < 2) error("No fPromela file provided.");

	if(!copyFile(argv[argc - 1], "__workingfile.tmp")) error("The fPromela file does not exist or is not readable!");

	std::string path;
	std::string ltlProp;
	std::string tvlFile;
	std::string tvlFilter;
	std::string pmlFile;
	std::string propFile;

	int i, ps = 0, check = 0, exec = 0, fm = 0, ltl = 0;

	tvl = new TVL();

	std::string argv_0 = std::string(argv[0]);
	path = argv_0.substr(0 , (argv_0.size() + 1) - sizeof("deadalux"));
	
	std::cout << path << std::endl;
	
	pmlFile = argv[argc - 1];
	if(!copyFile(argv[argc - 1], "__workingfile.tmp")) { std::cout << "The fPromela file does not exist or is not readable!\n"; exit(1); }

	// Read command-line arguments
	for(int i = 1; i < argc-1; i++) {
		if(strcmp(argv[i],"-s") == 0) {
			ps = 1;

		} else if(strcmp(argv[i],"-check") == 0) {
			check = 1;

		} else if(strcmp(argv[i],"-exhaustive") == 0) {
			exhaustive = 1;
        
        } else if(strcmp(argv[i],"-bfs") == 0) {
			bfs = 1;

		} else if(strcmp(argv[i],"-t") == 0) {
			keepTempFiles = 1;

		} else if(strcmp(argv[i],"-spin") == 0) {
			if(optimisedSpinMode) error("The options -spin and -ospin cannot be used together.");
			spinMode = 1;

		} else if(strcmp(argv[i],"-enum") == 0) {
			enum_ = 1;
		
		} else if(strcmp(argv[i],"-guided") == 0) {
			guided = 1;

		} else if(strcmp(argv[i],"-ospin") == 0) {
			if(spinMode) error("The options -spin and -ospin cannot be used together.");
			optimisedSpinMode = 1;
	    
	    } else if(strcmp(argv[i],"-sampling") == 0) {
            i++;
            if (i >= argc - 1) error("The -sampling option has to be followed by an integer denoting the number of executions to sample.");
            else sampling = atoi(argv[i]);
            if (sampling < 1) error("In -sampling mode, the number of executions to sample must be greater than zero.");
            
        } else if(strcmp(argv[i],"-ksteps") == 0) {
            i++;
            if (i >= argc - 1) error("The -ksteps option has to be followed by an integer denoting the number of steps of samples.");
            else ksteps = atoi(argv[i]);
            if (ksteps < 1) error("In -steps mode, the number of steps of samples must be greater than zero.");

		} else if(strcmp(argv[i],"-fdlc") == 0) {
			fullDeadlockCheck = 1;

		} else if(strcmp(argv[i],"-exec") == 0) {
			exec = 1;

		} else if(strcmp(argv[i],"-l") == 0) {
			i++;
			if(i >= argc - 1) error("The -l option has to be followed by an integer denoting the number of steps to execute.");
			else limitExploration = atol(argv[i]);

		} else if(strcmp(argv[i],"-filter") == 0) {
			i++;
			if(i >= argc - 1) error("The -filter option has to be followed by a TVL expression.");
			else tvlFilter = argv[i];

		} else if(strcmp(argv[i],"-fm") == 0) {
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

		} else if(strcmp(argv[i],"-nt") == 0) {
			trace = NO_TRACE;

		} else if(strcmp(argv[i],"-st") == 0) {
			if(trace != NO_TRACE) trace = SIMPLE_TRACE;

		} else if(strcmp(argv[i],"-ltl") == 0) {
			ltl = 1;
			i++;
			if(i >= argc - 1) error("The -ltl option has to be followed by an LTL property.");
			else {
				ltlProp = argv[i];
				std::string errorMsg;
				if(!appendClaim("__workingfile.tmp", path, ltlProp, errorMsg)) error("The LTL formula '%s' could not be parsed, error message: \n --\n%s\n", ltlProp, errorMsg);
			}

		} else if(strcmp(argv[i], "-sim") == 0) {
			if(check) error("The options -check and -sim cannot be used at the same time.");
			sim = 1;
			i++;
			pmlFile = argv[i];
		} else if(strcmp(argv[i], "-props") == 0) {
			i++;
			propFile = argv[i];
		} else if(strcmp(argv[i], "-stutter") == 0) {
			stutterStep = 1;
		}
		else if(strcmp(argv[i], "-bound") == 0) {
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

	srand(time(nullptr));

	//unsigned int index = program->assignMutables();
	//std::cout << "NUMBER OF MUTABLE NODE " << index << "\n";

	/*std::ofstream output;
	output.open("mutants/original.pml");
	output << "#include \"./Theory.prp\"\n";
	output << stmnt::string(program);
	output.close();*/

	//std::ofstream output;
	//output.open("output.pml");
	//output << "#include \"./Theory.prp\"\n";
	//output << stmnt::string(program);
	//output.close();

	ASTtoFSM* converter = new ASTtoFSM();
	fsm* automata = converter->astToFsm(globalSymTab, program, tvl);
	automata->orderAcceptingTransitions();

	//std::ofstream graph;
	//graph.open("fsm_graphvis");
	//automata->printGraphVis(graph);
	//graph.close();

	/*for(unsigned int i = 1; i <= index; i++) {
		auto copy = program->deepCopy();
		astNode::mutate(copy, i);
		output.open("mutants/mutant_"+ std::to_string(i) + ".pml");
		output << "#include \"./Theory.prp\"\n";
		output << stmnt::string(copy);
		output.close();
		delete copy;
	}*/

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

	/*auto loader = new promela_loader(pmlFile, tvlFile);
	auto program_graph = loader->getAutomata();

	auto mc = new ltlModelChecker(program_graph);
	auto result = mc->check(property);

	result->print();*/

	
	//state* init = new state(globalSymTab, automata);

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
