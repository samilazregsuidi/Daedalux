#include "modelchecking_subcommand.hpp"

void setup_subcommand_modelchecking(CLI::App &app)
{
	// Create the option and subcommand objects.
	auto opt = std::make_shared<ModelCheckingOptions>();
	auto sub = app.add_subcommand("modelchecking", "Model checking of a fPromela file");

	sub->add_option("-m, --model", opt->input_file, "Model to verify")
		->required()
		->check(CLI::ExistingFile);

	// Options for model checking
	sub->add_flag("-e, --exhaustive", opt->exhaustive, "Determines also which products have *no* problems. The normal check will stop at the first problem,  and does not determine whether there are products that have no problems  (e.g. those that satisfy a property).");
	sub->add_flag("-d, --fdlc", opt->fullDeadlockCheck, "Search for trivially invalid end states (more costly)");

	sub->add_flag("-c, --check", opt->check, "Verifies the model.  When a problem (assert, deadlock or property violation) is found, an error trace (counterexample) is printed and execution stops.");
	sub->add_flag("--bfs", opt->bfs, "Performs a breadth-first search instead of a depth-first search.");
	sub->add_option("--ksteps", opt->ksteps, "Bounded sampling to sample of lenght k.")->check(CLI::PositiveNumber);
	sub->add_option("-b, --bound", opt->bound, "Bounded sampling to sample of lenght k.")->check(CLI::PositiveNumber);
	sub->add_option("-s, --sampling", opt->sampleSize, "Sampling to sample of lenght k.")->check(CLI::PositiveNumber);
	sub->add_flag("--stutter", opt->stutterSteps, "Performs a stutter step search.");

	// Options for simulation
	sub->add_flag("--sim", opt->sim, "Simulates the model.  When a problem (assert, deadlock or property violation) is found, an error trace (counterexample) is printed and execution stops.");
	sub->add_flag("--enum", opt->enum_, "Iterate over every product of the product line.");
	sub->add_flag("--exec", opt->exec, "Executes the model.  When a problem (assert, deadlock or property violation) is found, an error trace (counterexample) is printed and execution stops.");

	sub->add_flag("--spin", opt->spinMode, "Treat features like normal variables (as SPIN would do).");
	sub->add_flag("--ospin", opt->optimisedSpinMode, "Similar to -spin, but statements with a bad guard are removed from the model before model checking.  The input is thus interpreted as fPromela (not exactly as SPIN would do). This is normally more efficient than -spin.");

	sub->add_option("-l, --limitExploration", opt->limitExploration, "Stop when the given number of states were explored. (This option can also be used for model checking.)")
		->check(CLI::PositiveNumber);

	sub->add_option("-f, --featuremodel", opt->tvl_file,
				   "Load the specified TVL file (only used in verification). This parameter can be omitted if the TVL file is named as the .pml file but with extension .tvl.")
		->check(CLI::ExistingFile);

	sub->add_option("--props", opt->propFile, "File containing the properties to check.")
		->check(CLI::ExistingFile);

	sub->add_option("--filter", opt->tvlFilter, "Limit the verification to a subset of the products  specified in the TVL file.  The TVL syntax has to be used for this.");

	sub->add_option("--fmdimacs", opt->tvl_files, "As before, but load the dimacs of the feature model directly.");

	// Output control options
	sub->add_flag("--nt", opt->noTraces, "Does not print any traces, only information  about  violating (or satisfying) products.");
	sub->add_flag("--st", opt->printInfo, "Prints information about the model and the verification process.");

	// Set the run function as callback to be called when this subcommand is issued.
	sub->callback([opt]()
				  { run_modelchecking(*opt); });
}

void run_modelchecking(ModelCheckingOptions const &opt)
{
	// Other global variables from main
	symTable *globalSymTab = nullptr;
	stmnt *program = nullptr;

	std::cout << "Model checking " << opt.input_file << std::endl;

	if (verify_modelchecking_options(opt) == false)
	{
		exit(1);
	}
	
	TVL* tvl = new TVL();

	if (!opt.tvl_files.first.empty() && !opt.tvl_files.second.empty())
	{
		tvl->loadFeatureModelDimacs(opt.tvl_files.first, opt.tvl_files.second);
	}

	// Some basic validity checks
	if (opt.tvl_file.empty() && !opt.tvlFilter.empty())
	{
		std::cout << "The -filter option cannot be used with the -fmdimacs option, only with -fm." << std::endl;
		exit(1);
	}
	if (!opt.tvl_file.empty())
	{
		if (!tvl->loadFeatureModel(opt.tvl_file, opt.tvlFilter)){
			std::cout << "Could not load the specified feature model file." << std::endl;
			exit(1);
		}
	}
	else if (opt.tvl_file.empty() && !opt.optimisedSpinMode)
	{
		// Try to guess name of feature model file name
		std::string tvlFile = std::string(opt.input_file).replace(opt.input_file.find(".pml"), 4, ".tvl");
		printf("tvl file = %s\n", tvlFile.c_str());

		if (!tvl->loadFeatureModel(tvlFile, opt.tvlFilter) && !opt.tvlFilter.empty())
		{
			std::cout << "The -filter option can only be used when a feature model is charged." << std::endl;
			exit(1);
		}
		else
		{
			if (!opt.tvl_file.empty() && (opt.spinMode || opt.optimisedSpinMode) && !opt.tvlFilter.empty())
			{
				std::cout << "The -filter option can only be used when a feature model is charged." << std::endl;
				exit(1);
			}
		}

		if (opt.propFile.empty() && (opt.sim || opt.stutterSteps))
		{
			std::cout << "Simulation checking and non stutter steps require a property file." << std::endl;
			exit(1);
		}
	}

	// Load promela file
	promela_loader loader(opt.input_file, tvl);
	fsm *automata = loader.getAutomata();

	// Initialize srand
	srand(time(nullptr));

	automata->orderAcceptingTransitions();

	int sum = 0;
	int index = 0;
	for (; index < 1; index++)
	{
		sum += launchExecutionMarkovChain(automata, tvl);
	}
	std::cout << sum << std::endl;
	double avg = (float)sum / index;
	std::cout << "proportion to satisfy the bltl property : " << avg << std::endl;

	/*for(int i = 0; i < NB_LASSO; ++i)
		findLasso(automata, K);*/

	// createStateSpaceBFS(automata, tvl);

	// ltlModelChecker* mc = new ltlModelChecker();
	// mc->startNestedDFS(automata, tvl);
	// delete mc;

	// createStateSpaceDFS_RR(automata, tvl);

	// createStateSpaceDFS(automata, tvl);

	std::ofstream symtable;
	std::string symtable_name = "sym_table_graphviz.dot";
	symtable.open(symtable_name);

	while (globalSymTab->prevSymTab())
		globalSymTab = globalSymTab->prevSymTab();

	globalSymTab->printGraphViz(symtable);
	symtable.close();

	// Clean up memory
	if (automata)
		delete automata;
	if (tvl)
		delete tvl;

	TVL::deleteBoolFct();

	// Delete program and symbol table
	delete program;
	delete globalSymTab;
}

bool verify_modelchecking_options(ModelCheckingOptions const &opt)
{
	bool valid = true;
	if (opt.spinMode && opt.optimisedSpinMode)
	{
		std::cout << "The options -spin and -ospin cannot be used together." << std::endl;
		valid = false;
	}
	if (opt.sampleSize > 0 && opt.ksteps > 0)
	{
		std::cout << "The options -sampling and -ksteps cannot be used together." << std::endl;
		valid = false;
	}
	if (opt.check && opt.exec)
	{
		std::cout << "The options -check and -exec cannot be used together." << std::endl;
		valid = false;
	}
	if (opt.check && opt.enum_)
	{
		std::cout << "The options -check and -enum cannot be used together." << std::endl;
		valid = false;
	}
	if (opt.check && !opt.propFile.empty())
	{
		std::cout << "The -check option cannot be used with the -props option." << std::endl;
		valid = false;
	}
	if (opt.tvl_file.find(".tvl") == std::string::npos)
	{
		std::cout << "The feature model file must have the extension .tvl." << std::endl;
		valid = false;
	}
	return valid;
}