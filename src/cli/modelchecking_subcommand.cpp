#include "modelchecking_subcommand.hpp"

void setup_subcommand_modelchecking(CLI::App & app)
{
  // Create the option and subcommand objects.
  auto opt = std::make_shared<ModelCheckingOptions>();
  auto sub = app.add_subcommand("check", "Model checking of a fPromela file");

  sub->add_option("-m, --model", opt->input_file, "Model to verify")->required()->check(CLI::ExistingFile);

  // Options for model checking
  sub->add_flag("-e, --exhaustive", opt->exhaustive,
                "Determines also which products have *no* problems. The normal check will stop at the first problem,  and does "
                "not determine whether there are products that have no problems  (e.g. those that satisfy a property).");

  sub->add_option("--ltl", opt->ltl, "LTL property to verify");
  sub->add_option("--multiLtl", opt->multiLtl, "MultiLTL property to verify");

  // sub->add_flag("-d, --fdlc", opt->fullDeadlockCheck, "Search for trivially invalid end states (more costly)");
	// Options for model checking
  sub->add_flag("-d, --fdlc", opt->fullDeadlockCheck, "Search for trivially invalid end states (more costly)");

  sub->add_flag("-v, --verify", opt->check,
                "Verifies the model.  When a problem (assert, deadlock or property violation) is found, an error trace "
                "(counterexample) is printed and execution stops.");
  // sub->add_flag("--bfs", opt->bfs, "Performs a breadth-first search instead of a depth-first search.");
  sub->add_option("--ksteps", opt->ksteps, "Bounded sampling to sample of lenght k.")->check(CLI::PositiveNumber);
  sub->add_option("-s, --sampling", opt->sampleSize, "Number of samples.")->check(CLI::PositiveNumber);
  // sub->add_flag("--stutter", opt->stutterSteps, "Performs a stutter step search.");

  // Options for simulation
  sub->add_flag("--sim", opt->sim,
                "Simulates the model.  When a problem (assert, deadlock or property violation) is found, an error trace "
                "(counterexample) is printed and execution stops.");
  // sub->add_flag("--enum", opt->enum_, "Iterate over every product of the product line.");
  sub->add_flag("--exec", opt->exec,
                "Executes the model.  When a problem (assert, deadlock or property violation) is found, an error trace "
                "(counterexample) is printed and execution stops.");


	sub->add_option("-f, --featuremodel", opt->tvl_file,
				   "Load the specified TVL file (only used in verification). This parameter can be omitted if the TVL file is named as the .pml file but with extension .tvl.");
		

  sub->add_option("--ltlFile", opt->ltlPropFile, "File containing the LTL properties to verify.")->check(CLI::ExistingFile);

  sub->add_option("--multiLtlFile", opt->multiLtlPropFile, "File containing the multiLTL properties to verify.")
      ->check(CLI::ExistingFile);

  // sub->add_option("--filter", opt->tvlFilter, "Limit the verification to a subset of the products  specified in the TVL file.
  // The TVL syntax has to be used for this."); sub->add_option("--fmdimacs", opt->tvl_files, "As before, but load the dimacs of
  // the feature model directly.");

  // Output control options
  sub->add_flag("--nt", opt->noTraces,
                "Does not print any traces, only information  about  violating (or satisfying) products.");
  sub->add_flag("--st", opt->printInfo, "Prints information about the model and the verification process.");

  // Set the run function as callback to be called when this subcommand is issued.
  sub->callback([opt]() { run_modelchecking(*opt); });
}

void run_modelchecking(ModelCheckingOptions const & opt)
{
  // Other global variables from main
  symTable * globalSymTab = nullptr;
  stmnt * program = nullptr;

  std::cout << "Model checking " << opt.input_file << std::endl;

  if (verify_modelchecking_options(opt) == false) {
    exit(1);
  }

  std::unique_ptr<TVL> tvl;

  // Some basic validity checks
  if (!opt.tvl_file.empty()) {
    if (!tvl->loadFeatureModel(opt.tvl_file, "")) {
      std::cout << "Could not load the specified feature model file." << std::endl;
      exit(1);
    }
  }
  else {
    // Try to guess name of feature model file name
    std::string tvlFile = std::string(opt.input_file).replace(opt.input_file.find(".pml"), 4, ".tvl");
    printf("tvl file = %s\n", tvlFile.c_str());
  }
  // Some basic validity checks
  
	if (!opt.tvl_file.empty())
	{
		if (!tvl->loadFeatureModel(opt.tvl_file, "")){
			std::cout << "Could not load the specified feature model file." << std::endl;
			exit(1);
		}
	}
	/*else if (opt.tvl_file.empty())
	{
		// Try to guess name of feature model file name
		std::string tvlFile = std::string(opt.input_file).replace(opt.input_file.find(".pml"), 4, ".tvl");
		printf("tvl file = %s\n", tvlFile.c_str());

    if (!tvl->loadFeatureModel(tvlFile, "")) {
      std::cout << "The -filter option can only be used when a feature model is charged." << std::endl;
      exit(1);
    }

    if (opt.ltlPropFile.empty() && (opt.sim)) {
      std::cout << "Simulation checking and non stutter steps require a property file." << std::endl;
      exit(1);
    }
  }
		if (opt.ltlPropFile.empty() && (opt.sim))
		{
			std::cout << "Simulation checking and non stutter steps require a property file." << std::endl;
			exit(1);
		}
	}*/

  // Load promela file
  auto loader = std::make_unique<promela_loader>(opt.input_file, tvl.get());
  auto automata = loader->getAutomata();

  // Initialize srand
  srand(time(nullptr));

  automata->orderAcceptingTransitions();

  createStateSpaceDFS(automata.get(), tvl.get());

  // Delete program and symbol table
  delete program;
  delete globalSymTab;
}




bool verify_modelchecking_options(ModelCheckingOptions const & opt)
{
	bool valid = true;
	/*if(!opt.check) {
		if(opt.ltl.empty() && opt.ltlPropFile.empty()){
			if(opt.multiLtl.empty() && opt.multiLtlPropFile.empty()){
				std::cout << "No properties to verify have been specified" << std::endl;
				valid = false;
			}
		} 
	} else {
		valid = !(opt.ltl.empty() && opt.ltlPropFile.empty() && opt.multiLtl.empty() && opt.multiLtlPropFile.empty());
	}*/

  if (opt.sampleSize > 0 && opt.ksteps > 0) {
    std::cout << "The options -sampling and -ksteps cannot be used together." << std::endl;
    valid = false;
  }
  if (opt.check && opt.exec) {
    std::cout << "The options -check and -exec cannot be used together." << std::endl;
    valid = false;
  }
  /*if (opt.tvl_file.find(".tvl") == std::string::npos) {
    std::cout << "The feature model file must have the extension .tvl." << std::endl;
    valid = false;
  }*/
	
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
	/*if (opt.tvl_file.find(".tvl") == std::string::npos)
	{
		std::cout << "The feature model file must have the extension .tvl." << std::endl;
		valid = false;
	}*/
	return valid;
}