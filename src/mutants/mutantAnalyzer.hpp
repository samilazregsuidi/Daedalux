
#include "../core/semantic/variable/state/state.hpp"
#include <memory>
#include <string>
#include <vector>

class MutantAnalyzer {
public:
  // Constructor
  MutantAnalyzer(std::string original_file_path, std::string property_file_path, std::vector<std::string> mutant_file_paths);

  MutantAnalyzer(std::string original_file_path, std::vector<std::string> mutant_file_paths);

  MutantAnalyzer(std::string original_file_path, std::string property_file_path);

  MutantAnalyzer(std::string original_file_path);

  std::pair<std::vector<std::string>, std::vector<std::string>> killMutants(void);

  void analyzeSpecification(unsigned int number_of_mutants, unsigned int trace_size);

  void createMutants(unsigned int number_of_mutants);

  void analyzeMutants(unsigned int number_of_mutants);

  std::vector<std::string> getMutantFilePaths() { return mutant_file_paths; }
  std::string getOriginalFilePath() { return original_file_path; }

private:
  std::string original_file_path;
  std::string property_file_path;
  std::vector<std::string> mutant_file_paths;

  std::string createMutant(int mutant_number, stmnt * program, std::string mutant_file_path);

  bool fileExists(const std::string & filename)
  {
    std::ifstream file(filename);
    return file.good(); // Returns true if the file exists, false otherwise.
  }
};