#pragma once

#include "../core/semantic/variable/state/state.hpp"
#include "../formulas/formula.hpp"
#include <memory>
#include <string>
#include <vector>

class MutantAnalyzer {
public:
  // Constructor
  MutantAnalyzer(const std::string & original_file_path, const std::string & property_file_path, std::vector<std::string> mutant_file_paths);

  MutantAnalyzer(const std::string & original_file_path, std::vector<std::string> mutant_file_paths);

  MutantAnalyzer(const std::string & original_file_path, const std::string & property_file_path);

  explicit MutantAnalyzer(const std::string & original_file_path);

  std::pair<std::vector<std::string>, std::vector<std::string>> killMutants(void);

  void enhanceSpecification(unsigned int number_of_mutants);

  void createMutants(unsigned int number_of_mutants);

  std::map<std::string, std::shared_ptr<formula>> analyzeMutants(void);

  std::vector<std::string> getMutantFilePaths() const { return mutant_file_paths; }
  std::string getOriginalFilePath() const { return original_file_path; }

private:
  const std::string & original_file_path;
  std::string property_file_path;
  std::vector<std::string> mutant_file_paths;

  std::string createMutant(int mutant_number, const stmnt * program, const std::string & mutant_file_path);

  bool fileExists(const std::string & filename)
  {
    std::ifstream file(filename);
    return file.good(); // Returns true if the file exists, false otherwise.
  }
};