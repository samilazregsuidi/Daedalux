#include "mutant_subcommand.hpp"

void setup_subcommand_mutations(CLI::App & app)
{

  // Create the option and subcommand objects.
  auto opt = std::make_shared<MutantsOptions>();
  auto sub = app.add_subcommand("gen-mutants", "Generate mutants from a Promela file");

  sub->add_option("-f, --file", opt->input_file, "Promela file to mutate")->check(CLI::ExistingFile)->required();
  sub->add_option("-p, --property", opt->property_file, "Property file to mutate")->check(CLI::ExistingFile)->required();
  sub->add_option("-n, --nb_mutants", opt->number_of_mutants, "Number of mutants to generate")
      ->check(CLI::Range(1, 200))
      ->required();

  // Set the run function as callback to be called when this subcommand is issued.
  sub->callback([opt]() { generate_mutants(*opt); });

  auto genTopt = std::make_shared<GenTracesOptions>();

  sub = app.add_subcommand("gen-traces", "Generate positives and negatives traces from an original and a mutant file");
  sub->add_option("-o, --original", genTopt->original_file, "Original promela file to explore")->check(CLI::ExistingFile)->required();
  sub->add_option("-m, --mutant", genTopt->mutant_file, "Mutant promela file to explore")->check(CLI::ExistingFile)->required();
  sub->add_option("-l, --l_traces", genTopt->traces_length, "Length of traces")->required();
  sub->add_option("-n, --nb_traces", genTopt->nb_traces, "Number of traces to generate")->required();

  sub->callback([genTopt]() { generateMutantTraces(genTopt->original_file, genTopt->mutant_file, genTopt->traces_length, genTopt->nb_traces); });

}

void generate_mutants(const MutantsOptions & opt)
{
  std::cout << "Generating " << opt.number_of_mutants << " mutants from " << opt.input_file << std::endl;
  // Load promela file
  auto loader = std::make_unique<promela_loader>(opt.input_file, nullptr);
  stmnt * program = loader->getProgram();

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
  // Generate traces for mutants
  generateMutantTraces(opt.input_file, mutant_folder + "/mutant_1.pml", 100, 100);
}

bool fileExists(const std::string & filename)
{
  std::ifstream file(filename);
  return file.good(); // Returns true if the file exists, false otherwise.
}

void generateMutantTraces(const std::string& original_file, const std::string& mutant_file, size_t traces_length, unsigned int number_of_traces)
{
  // Assert that both files exist before generating traces
  assert(fileExists(mutant_file));
  assert(fileExists(original_file));

  // Load promela files using smart pointers
  std::unique_ptr<promela_loader> loader_mutant = std::make_unique<promela_loader>(mutant_file, nullptr);

  std::shared_ptr<fsm> fsm_mutant = loader_mutant->getAutomata();

  std::unique_ptr<promela_loader> loader_original = std::make_unique<promela_loader>(original_file, nullptr);
  auto fsm_original = loader_original->getAutomata();

  // Generate traces
  std::unique_ptr<traceReport> report = generateTraces(fsm_original, fsm_mutant);

  // Write traces to file
  std::ofstream negative_output;
  std::ofstream positive_output;
  negative_output.open(mutant_file.substr(0, mutant_file.find_last_of(".")) + "_negative.csv");
  positive_output.open(mutant_file.substr(0, mutant_file.find_last_of(".")) + "_positive.csv");

  report->printCSV(positive_output, negative_output);
  negative_output.close();
  positive_output.close();

  std::cout << "Generated traces" << std::endl;

  // Memory cleanup is automatic with smart pointers
}

/* Generate featured mutant models from a promela file
 * @param original: original promela file
 * @param number_of_mutants: number of mutants to generate
 */
// fsm generateFeaturedMutants(const std::string& original_file, unsigned int number_of_mutants)
// {
//   std::unique_ptr<promela_loader> loader_original = std::make_unique<promela_loader>(original_file, nullptr);
//   auto fsm_original(loader_original->getAutomata());

//   // Generate mutants
//   for (unsigned int j = 0; j < number_of_mutants; j++) {
//     // Generate mutant by mutating the original program and writing it to a file
//   }
//   return nullptr
// }

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
      // killed_mutants.push_back(property);
      // surviving_mutants.erase(std::remove(surviving_mutants.begin(), surviving_mutants.end(), property),
      // surviving_mutants.end());
    }
    else {
      std::cout << "Property does not kill mutant" << std::endl;
    }
  }

  // Shrink the feature model to only surviving mutants by removing all transitions with a featured expression that is not in
  // the surviving mutants

  // Save killed mutants to file
}