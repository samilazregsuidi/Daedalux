#pragma once

#include <string>
#include "CLI11.hpp"
#include <fstream>

#include "promela_loader.hpp"
#include "explore.hpp"

/// Collection of all options of Subcommand A.
struct MutantsOptions {
    unsigned int number_of_mutants;
	std::string input_file;
	std::string property_file;
};

struct GenTracesOptions {
	std::string original_file;
	std::string mutant_file;
	size_t traces_length;
	unsigned int nb_traces;
};

struct GenSingleTracesOptions {
	std::string original_file;
	size_t traces_length;
	unsigned int nb_traces;
};

// Function declarations.
void setup_subcommand_mutations(CLI::App &app);
void generate_mutants(const MutantsOptions& opt);
void generateMutantTraces(const std::string& original, const std::string& mutant_file, size_t traces_length, unsigned int traces_number);
void generateSingleTraces(const std::string& original, size_t traces_length, unsigned int traces_number);
