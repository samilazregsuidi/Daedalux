
#include "mutantAnalyzer.hpp"
#include "../formulas/formulaCreator.hpp"
#include "promela_loader.hpp"
#include "traceGenerator.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Constructor
MutantAnalyzer::MutantAnalyzer(std::string original_file_path, std::string property_file_path,
                               std::vector<std::string> mutant_file_paths)
    : original_file_path(original_file_path), property_file_path(property_file_path), mutant_file_paths(mutant_file_paths)
{
  if (!fileExists(original_file_path)) {
    std::cout << "Original file " << original_file_path << " does not exist" << std::endl;
    throw std::invalid_argument("Original file " + original_file_path + " does not exist");
  }
  if (!fileExists(property_file_path)) {
    std::cout << "Property file " << property_file_path << " does not exist" << std::endl;
    throw std::invalid_argument("Property file " + property_file_path + " does not exist");
  }
  for (auto mutant_file_path : mutant_file_paths) {
    if (!fileExists(mutant_file_path)) {
      std::cout << "Mutant file " << mutant_file_path << " does not exist" << std::endl;
      throw std::invalid_argument("Mutant file " + mutant_file_path + " does not exist");
    }
  }
}

MutantAnalyzer::MutantAnalyzer(std::string original_file_path, std::vector<std::string> mutant_file_paths)
    : original_file_path(original_file_path), mutant_file_paths(mutant_file_paths)
{
  if (!fileExists(original_file_path)) {
    std::cout << "Original file " << original_file_path << " does not exist" << std::endl;
    throw std::invalid_argument("Original file " + original_file_path + " does not exist");
  }

  for (auto mutant_file_path : mutant_file_paths) {
    if (!fileExists(mutant_file_path)) {
      std::cout << "Mutant file " << mutant_file_path << " does not exist" << std::endl;
      throw std::invalid_argument("Mutant file " + mutant_file_path + " does not exist");
    }
  }
}

MutantAnalyzer::MutantAnalyzer(std::string original_file_path, std::string property_file_path)
    : original_file_path(original_file_path), property_file_path(property_file_path)
{
  if (!fileExists(original_file_path)) {
    std::cout << "Original file " << original_file_path << " does not exist" << std::endl;
    throw std::invalid_argument("Original file " + original_file_path + " does not exist");
  }
  if (!fileExists(property_file_path)) {
    std::cout << "Property file " << property_file_path << " does not exist" << std::endl;
    throw std::invalid_argument("Property file " + property_file_path + " does not exist");
  }
}

MutantAnalyzer::MutantAnalyzer(std::string original_file_path) : original_file_path(original_file_path)
{
  if (!fileExists(original_file_path)) {
    std::cout << "Original file does not exist" << std::endl;
    throw std::invalid_argument("Original file does not exist");
  }
}

void MutantAnalyzer::analyzeSpecification(unsigned int number_of_mutants, unsigned int trace_size)
{
  std::cout << "Analyzing specification using mutation testing" << std::endl;
  // Create mutants
  createMutants(number_of_mutants);

  // Kill mutants
  auto [killed_mutants, surviving_mutants] = killMutants();

  // Analyze surviving mutants using trace generation and comparison with the original program
  analyzeMutants(trace_size);

  std::cout << "Finished analyzing specification using mutation testing" << std::endl;
}

//*
// * This method kills the mutants by checking whether the properties kill them
// */
std::pair<std::vector<std::string>, std::vector<std::string>> MutantAnalyzer::killMutants()
{
  auto killed_mutants = std::vector<std::string>();
  auto surviving_mutants = std::vector<std::string>();
  // One by one, check whether the properties kill the mutants
  for (auto mutant_file_path : mutant_file_paths) {
    // Load promela files using smart pointers
    std::unique_ptr<promela_loader> loader_mutant = std::make_unique<promela_loader>(mutant_file_path, nullptr);
    std::shared_ptr<fsm> fsm_mutant = loader_mutant->getAutomata();

    bool is_killed = false;

    // Check whether the properties kill the mutant
    // TODO implement - not sure how to do this yet - SAMI might be able to help with this

    // If the mutant is killed, add it to the list of killed mutants and add it to the list of surviving mutants otherwise
    if (is_killed) {
      std::cout << "Mutant " << mutant_file_path << " is killed" << std::endl;
      killed_mutants.push_back(mutant_file_path);
    }
    else {
      std::cout << "Mutant " << mutant_file_path << " is surviving" << std::endl;
      surviving_mutants.push_back(mutant_file_path);
    }
  }
  return std::make_pair(killed_mutants, surviving_mutants);
}

void MutantAnalyzer::createMutants(unsigned int number_of_mutants)
{
  // Load promela file
  auto loader = std::make_unique<promela_loader>(original_file_path, nullptr);
  stmnt * program = loader->getProgram();
  unsigned int index = program->assignMutables();
  // Folder of the original program
  std::string folder = original_file_path.substr(0, original_file_path.find_last_of("/"));
  std::string mutant_folder = folder + "/mutants";
  // std::string property_file_name = opt.property_file.substr(opt.input_file.find_last_of("/") + 1);

  // Write original program to file so that it can be used by the mutation operators
  // Create folder for mutants if it does not exist
  if (!std::filesystem::exists(mutant_folder)) {
    std::filesystem::create_directory(mutant_folder);
    std::cout << "Created folder " << mutant_folder << " for mutants" << std::endl;
  }

  std::ofstream output;
  output.open(mutant_folder + "/original.pml");
  //   output << "#include \"../" << property_file_name << "\"" << std::endl << std::endl;
  output << stmnt::string(program);
  output.close();

  // Generate mutants
  number_of_mutants = std::min(number_of_mutants, index);
  for (unsigned int i = 1; i <= number_of_mutants; i++) {
    auto fileName = createMutant(i, program, mutant_folder);
    mutant_file_paths.push_back(fileName);
  }
  std::cout << "Generated " << mutant_file_paths.size() << " mutants" << std::endl;
}

void MutantAnalyzer::analyzeMutants(unsigned int trace_size)
{
  std::cout << "Analyzing mutants" << std::endl;
  // Load original promela file
  auto original_loader = new promela_loader(original_file_path, nullptr);
  auto originalFSM = original_loader->getAutomata();
  delete original_loader;
  // Loop through all mutants and compare them to the original one by one
  for (auto mutant_file_path : mutant_file_paths) {
    auto mutant_loader = std::make_unique<promela_loader>(mutant_file_path, nullptr);
    auto mutantFSM = mutant_loader->getAutomata();
    auto traceGen = std::make_unique<TraceGenerator>(originalFSM, mutantFSM);
    auto trace = traceGen->generateTraceReport(1, trace_size);
    auto goodTrace = *trace->getGoodTraces().begin();
    auto badTrace = *trace->getBadTraces().begin();
    formulaCreator::distinguishTraces(goodTrace, badTrace);
  }
}

std::string MutantAnalyzer::createMutant(int mutant_number, stmnt * program, std::string mutant_folder)
{
  auto copy = program->deepCopy();
  astNode::mutate(copy, mutant_number);
  std::string name = "mutant_" + std::to_string(mutant_number);
  std::string file_name = mutant_folder + "/" + name + ".pml";
  std::ofstream output;
  // Write property file to mutant folder as well
  //   output << "#include \"../" << property_file_name << "\"" << std::endl << std::endl;
  output.open(file_name);
  output << stmnt::string(copy);
  output.close();
  delete copy;
  return file_name;
}