#pragma once

#include "../core/semantic/variable/state/state.hpp"
#include "../formulas/formula.hpp"
#include <memory>
#include <string>
#include <vector>
using CheckFunction = std::function<bool(const std::string &, std::shared_ptr<formula>)>;
using SurvivorKilledPair = std::pair<std::vector<std::string>, std::vector<std::string>>;

class MutantAnalyzer {
public:
  // Constructor
  MutantAnalyzer(const std::string & original_file_path, const std::string & property_file_path,
                 std::vector<std::string> mutant_file_paths);

  MutantAnalyzer(const std::string & original_file_path, std::vector<std::string> mutant_file_paths);

  MutantAnalyzer(const std::string & original_file_path, const std::string & property_file_path);

  explicit MutantAnalyzer(const std::string & original_file_path);

  SurvivorKilledPair killMutants(void);
  SurvivorKilledPair killMutantsSpin(std::shared_ptr<formula> f);

  std::shared_ptr<formula> enhanceSpecification(unsigned int number_of_mutants);

  void createMutants(unsigned int number_of_mutants, bool renameVariables = false);

  std::map<std::string, std::shared_ptr<formula>> analyzeMutants(void);

  void generateScarletFile(const std::string & file_path, unsigned int number_of_mutants,
                           const std::vector<std::shared_ptr<statePredicate>> & predicates);

  void generatePredicatesForScarletFile(const std::string & file_path, unsigned int number_of_mutants);

  std::vector<std::string> getMutantFilePaths() const { return mutant_file_paths; }
  std::string getOriginalFilePath() const { return original_file_path; }

private:
  const std::string & original_file_path;
  std::string property_file_path;
  std::vector<std::string> mutant_file_paths;

  std::string createMutant(int mutant_number, const stmnt * program, const std::string & mutant_file_path);

  SurvivorKilledPair checkMutants(CheckFunction checkFunction, std::shared_ptr<formula> f);

  bool fileExists(const std::string & filename)
  {
    std::ifstream file(filename);
    return file.good(); // Returns true if the file exists, false otherwise.
  }
};