#include "mutant_subcommand.hpp"
#include "traceGenerator.hpp"
#include "../mutants/mutantAnalyzer.hpp"

void setup_subcommand_mutations(CLI::App & app)
{

  // Create the option and subcommand objects.
  auto opt = std::make_shared<MutantsOptions>();
  auto sub = app.add_subcommand("gen-mutants", "Generate mutants from a Promela file");

  sub->add_option("-f, --file", opt->input_file, "Promela file to mutate")->check(CLI::ExistingFile)->required();
  sub->add_option("-n, --nb_mutants", opt->number_of_mutants, "Number of mutants to generate")
      ->check(CLI::Range(1, 200))
      ->required();

  // Set the run function as callback to be called when this subcommand is issued.
  sub->callback([opt]() { generate_mutants(*opt); });

  auto genTraceOpt = std::make_shared<GenTracesOptions>();
  sub = app.add_subcommand("gen-traces", "Generate positives and negatives traces from an original and a mutant file");
  sub->add_option("-o, --original", genTraceOpt->original_file, "Original promela file to explore")->check(CLI::ExistingFile)->required();
  sub->add_option("-m, --mutant", genTraceOpt->mutant_file, "Mutant promela file to explore")->check(CLI::ExistingFile)->required();
  sub->add_option("-l, --l_traces", genTraceOpt->traces_length, "Length of traces")->required();
  sub->add_option("-n, --nb_traces", genTraceOpt->nb_traces, "Number of traces to generate")->required();

  sub->callback([genTraceOpt]() { generateMutantTraces(genTraceOpt->original_file, genTraceOpt->mutant_file, genTraceOpt->traces_length, genTraceOpt->nb_traces); });

  auto traceOpt = std::make_shared<GenSingleTracesOptions>();
  auto sub2 = app.add_subcommand("gen-single-traces", "Generate traces from a single promela file");
  sub2->add_option("-f, --file", traceOpt->original_file, "Promela file to explore")->check(CLI::ExistingFile)->required();
  sub2->add_option("-l, --l_traces", traceOpt->traces_length, "Length of traces")->required();
  sub2->add_option("-n, --nb_traces", traceOpt->nb_traces, "Number of traces to generate")->required();

  sub2->callback([traceOpt]() { generateSingleTraces(traceOpt->original_file, traceOpt->traces_length, traceOpt->nb_traces); });

}

void generate_mutants(const MutantsOptions & opt)
{
  std::cout << "Generating " << opt.number_of_mutants << " mutants from " << opt.input_file << std::endl;
  // Load promela file
  MutantAnalyzer mutantAnalyzer(opt.input_file);
  mutantAnalyzer.createMutants(opt.number_of_mutants);

  auto mutants = mutantAnalyzer.getMutantFilePaths();

  // Write mutant file names to file
  std::ofstream mutants_file;
  mutants_file.open(opt.input_file.substr(0, opt.input_file.find_last_of(".")) + "_mutants.txt");
  for (auto mutant : mutants) {
    std::cout << mutant << std::endl;
    mutants_file << mutant << std::endl;
  }
  mutants_file.close();

  std::cout << "Generated mutants" << std::endl;
}

bool fileExists(const std::string & filename)
{
  std::ifstream file(filename);
  return file.good(); // Returns true if the file exists, false otherwise.
}

void generateSingleTraces(const std::string& file, size_t trace_length, unsigned int traces_number){
  assert(fileExists(file));

  std::unique_ptr<promela_loader> loader = std::make_unique<promela_loader>(file, nullptr);
  auto fsm(loader->getAutomata());

  for(unsigned int i = 0; i < traces_number; i++){
    auto trace = TraceGenerator::generateTrace(fsm, trace_length);
    std::ofstream output;
    auto fileName = file.substr(0, file.find_last_of(".")) + "_trace_" + std::to_string(i) + ".csv";
    std::cout << fileName << std::endl;
    output.open(fileName);
    trace->printCSV(output);
    output.close();
  }
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

  auto traceGen = std::make_unique<TraceGenerator>(fsm_original, fsm_mutant);

  // Generate traces
  std::unique_ptr<traceReport> report = traceGen->generateTraceReport(number_of_traces, traces_length);

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
