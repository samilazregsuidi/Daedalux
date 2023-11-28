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

// Function declarations.
void setup_subcommand_mutations(CLI::App &app);
void generate_mutants(MutantsOptions const &opt);
void generateMutantTraces(std::string original, std::string mutant_file);
