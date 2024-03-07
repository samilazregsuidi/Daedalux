
#include "mutantAnalyzer.hpp"
#include "../formulas/formulaCreator.hpp"
#include "explore.hpp"
#include "ltlModelChecker.hpp"
#include "promela_loader.hpp"
#include "fsmExplorer.hpp"
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

void MutantAnalyzer::enhanceSpecification(unsigned int number_of_mutants, unsigned int trace_size)
{
  std::cout << "Enhance specification using mutation testing" << std::endl;
  // Create mutants
  // createMutants(number_of_mutants);
  // Filter out bisimilar mutants
  // TODO implement - not sure how to do this yet - SAMI might be able to help with this

  // Kill mutants
  auto [killed_mutants, surviving_mutants] = killMutants();

  if (surviving_mutants.empty()) {
    std::cout << "All mutants are killed, no need to enhance the specification" << std::endl;
    return;
  }

  // Analyze surviving mutants using trace generation and comparison with the original program
  auto formulas = analyzeMutants(trace_size);

  std::vector<std::shared_ptr<formula>> formulas_vector = {};

  // Print results
  for (auto [mutant, formula] : formulas) {
    std::cout << "Mutant " << mutant << " can be distinguished from the original program using the formula "
              << formula->toFormula() << std::endl;
    formulas_vector.push_back(formula);
  }

  // Combine formulas using the && operator
  auto combined_formula = formulaUtility::combineFormulas(formulas_vector, CombinationOperatorType::AND_Symbol);

  // Simplify the formula using the OWL tool

  std::string formula_string = combined_formula->promelaFormula();
  std::string definition_string = combined_formula->getDefinition();

  // Append the formula to both the original and the mutant files
  LTLClaimsProcessor::renewClaimOfFile(original_file_path, definition_string, formula_string);

  for (auto filePath : mutant_file_paths) {
    LTLClaimsProcessor::renewClaimOfFile(filePath, definition_string, formula_string);
  }

  // Add the formula to the property file - for now, just print it
  std::cout << "The enhanced specification is " << combined_formula->toFormula() << std::endl;
}

bool checkPromelaModel(const std::string & file_path)
{
  promela_loader * loader = new promela_loader(file_path, nullptr);
  ltlModelChecker * mc = new ltlModelChecker();
  std::shared_ptr<fsm> fsm = loader->getAutomata();
  delete loader;
  bool model_correct = true;
  try {
    model_correct = mc->check(fsm.get(), nullptr);
  }
  catch (const std::exception & e) {
    std::cerr << e.what() << '\n';
  }
  delete mc;
  return model_correct;
}

//*
// * This method kills the mutants by checking whether the properties kill them
// */
std::pair<std::vector<std::string>, std::vector<std::string>> MutantAnalyzer::killMutants()
{
  auto killed_mutants = std::vector<std::string>();
  auto surviving_mutants = std::vector<std::string>();
  // One by one, check whether the already specified properties kill the mutants
  for (auto mutant_file_path : mutant_file_paths) {
    // Check whether the mutant is correct
    bool model_correct = checkPromelaModel(mutant_file_path);
    // If the mutant is killed, add it to the list of killed mutants and add it to the list of surviving mutants otherwise
    if (model_correct) {
      std::cout << "Mutant " << mutant_file_path << " is surviving" << std::endl;
      surviving_mutants.push_back(mutant_file_path);
    }
    else {
      std::cout << "Mutant " << mutant_file_path << " is killed" << std::endl;
      killed_mutants.push_back(mutant_file_path);
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

  // Write original program to file so that it can be used by the mutation operators
  // Create folder for mutants if it does not exist
  if (!std::filesystem::exists(mutant_folder)) {
    std::filesystem::create_directory(mutant_folder);
    std::cout << "Created folder " << mutant_folder << " for mutants" << std::endl;
  }
  else {
    std::cout << "Folder " << mutant_folder << " for mutants already exists. Deleting it and creating a new one" << std::endl;
    std::filesystem::remove_all(mutant_folder);
    std::filesystem::create_directory(mutant_folder);
  }

  std::ofstream output;
  output.open(mutant_folder + "/original.pml");
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

std::map<std::string, std::shared_ptr<formula>> MutantAnalyzer::analyzeMutants(unsigned int trace_size)
{
  // Load original promela file
  auto original_loader = new promela_loader(original_file_path, nullptr);
  auto originalFSM = original_loader->getAutomata();
  delete original_loader;
  auto resultMap = std::map<std::string, std::shared_ptr<formula>>();
  // Loop through all mutants and compare them to the original one by one
  for (auto mutant_file_path : mutant_file_paths) {
    auto mutant_loader = std::make_unique<promela_loader>(mutant_file_path, nullptr);
    auto mutantFSM = mutant_loader->getAutomata();
    std::shared_ptr<formula> formula = fsmExplorer::discardMutant(originalFSM, mutantFSM); // Remove the unnecessary 'auto' keyword
    resultMap[mutant_file_path] = formula;
  }
  std::cout << "Finished analyzing mutants" << std::endl;
  return resultMap;
}

std::string MutantAnalyzer::createMutant(int mutant_number, stmnt * program, std::string mutant_folder)
{
  auto copy = program->deepCopy();
  astNode::mutate(copy, mutant_number);
  std::string name = "mutant_" + std::to_string(mutant_number);
  std::string file_name = mutant_folder + "/" + name + ".pml";
  std::ofstream output;
  output.open(file_name);
  output << stmnt::string(copy);
  output.close();
  delete copy;
  return file_name;
}