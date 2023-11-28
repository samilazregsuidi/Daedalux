#include "mutant_subcommand.hpp"

void setup_subcommand_mutations(CLI::App & app)
{

  // Create the option and subcommand objects.
  auto opt = std::make_shared<MutantsOptions>();
  auto sub = app.add_subcommand("mutants", "Generate mutants from a Promela file");

  app.add_option("-f, --file", opt->input_file, "Promela file to mutate")->check(CLI::ExistingFile)->required();
  app.add_option("-p, --property", opt->property_file, "Property file to mutate")->check(CLI::ExistingFile)->required();
  app.add_option("-n, --no_mutants", opt->number_of_mutants, "Number of mutants to generate")
      ->check(CLI::Range(1, 200))
      ->required();

  // Set the run function as callback to be called when this subcommand is issued.
  sub->callback([opt]() { generate_mutants(*opt); });
}

void generate_mutants(MutantsOptions const & opt)
{
  std::cout << "Generating " << opt.number_of_mutants << " mutants from " << opt.input_file << std::endl;
  // Load promela file
  promela_loader * loader = new promela_loader();
  loader->load_promela_file(opt.input_file, nullptr);
  stmnt * program = loader->get_program();

  unsigned int index = program->assignMutables();
  std::cout << "NUMBER OF MUTABLE NODES " << index << std::endl;

  // Folder of the original program
  std::string folder = opt.input_file.substr(0, opt.input_file.find_last_of("/"));
  std::string mutant_folder = folder + "/mutants";
  std::string property_file_name = opt.property_file.substr(opt.input_file.find_last_of("/") + 1);

  // Write original program to file so that it can be used by the mutation operators
  // Create folder for mutants if it does not exist
  if (!std::filesystem::exists(mutant_folder)) {
    std::filesystem::create_directory(mutant_folder);
    std::cout << "Created folder " << mutant_folder << std::endl;
  }

  std::ofstream output;
  output.open(mutant_folder + "/original.pml");
  output << "#include \"../" << property_file_name << "\"" << std::endl << std::endl;
  output << stmnt::string(program);
  output.close();

  // Generate mutants
  for (unsigned int j = 0; j < opt.number_of_mutants; j++) {
    for (unsigned int i = 1; i <= index; i++) {
      // Generate mutant by mutating the original program and writing it to a file
      auto copy = program->deepCopy();
      astNode::mutate(copy, i);
      std::string file_name = mutant_folder + "/mutant_" + std::to_string(j * index + i) + ".pml";
      output.open(file_name);
      // Write property file to mutant folder as well
      output << "#include \"../" << property_file_name << "\"" << std::endl << std::endl;
      output << stmnt::string(copy);
      output.close();
      delete copy;
    }
  }

  std::cout << "Generated " << opt.number_of_mutants * index << " mutants" << std::endl;
  delete loader;

  // Generate traces for mutants
  generateMutantTraces(opt.input_file, mutant_folder + "/mutant_1.pml");
}

bool fileExists(const std::string & filename)
{
  std::ifstream file(filename);
  return file.good(); // Returns true if the file exists, false otherwise.
}

void generateMutantTraces(std::string original, std::string mutant_file)
{
  // Assert that both files exist before generating traces
  assert(fileExists(mutant_file));
  assert(fileExists(original));

  // Load promela files
  promela_loader * loader_mutant = new promela_loader();
  auto fsm_mutant = loader_mutant->load_promela_file(mutant_file, nullptr);
  delete loader_mutant;
  promela_loader * loader_original = new promela_loader();
  auto fsm_original = loader_original->load_promela_file(original, nullptr);
  delete loader_original;

  // Generate traces
  traceReport report = generateTraces(fsm_original, fsm_mutant);

  // Write traces to file
  std::ofstream negative_output;
  std::ofstream positive_output;
  negative_output.open(mutant_file.substr(0, mutant_file.find_last_of(".")) + "_negative.csv");
  positive_output.open(mutant_file.substr(0, mutant_file.find_last_of(".")) + "_positive.csv");
  report.printCSV(positive_output, negative_output);
  negative_output.close();
  positive_output.close();

  std::cout << "Generated traces" << std::endl;

  // Clean up memory
  if (fsm_mutant)
    delete fsm_mutant;
  if (fsm_original)
    delete fsm_original;
}

/* Generate featured mutant models from a promela file
 * @param original: original promela file
 * @param number_of_mutants: number of mutants to generate
 */
fsm generateFeaturedMutants(std::string original, unsigned int number_of_mutants)
{
  promela_loader * loader_original = new promela_loader();
  auto fsm_original = loader_original->load_promela_file(original, nullptr);
  delete loader_original;

  // Generate mutants
  for (unsigned int j = 0; j < number_of_mutants; j++) {
    // Generate mutant by mutating the original program and writing it to a file

  }
}

/* Analyze mutants to determine whether the specification will kill them or not
 * @param feature_mutant_model: mutant model as a featured transition system
 * @param properties: properties to check
 */
void analyzeMutants(fsm feature_mutant_model, std::vector<std::string> properties)
{
  // Initally, all mutants are surviving mutants
  auto killed_mutants = std::vector<std::string>();
  auto surviving_mutants = std::vector<std::string>();	
  // Check whether the mutants are killed by the properties
  for (auto property : properties) {
	std::cout << "Checking property " << property << std::endl;
	// Check whether the property kills the mutant
	// TODO implement - not sure how to do this yet 
	bool killed = false;
	if (killed) {
	  std::cout << "Property kills mutant" << std::endl;
	  // Move mutant from surviving to killed mutants
	  //killed_mutants.push_back(property);
	  //surviving_mutants.erase(std::remove(surviving_mutants.begin(), surviving_mutants.end(), property), surviving_mutants.end());
	} else {
		std::cout << "Property does not kill mutant" << std::endl;	
	}
  }

  // Shrink the feature model to only surviving mutants by removing all transitions with a featured expression that is not in the surviving mutants


  // Save killed mutants to file

}