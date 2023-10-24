#pragma once

#include <string>
#include "CLI11.hpp"

#include "promela_loader.hpp"

/// Collection of all options of Subcommand A.
struct MutantsOptions {
    int number_of_mutants;
	std::string input_file;
	std::string property_file;
};

// Function declarations.
void setup_subcommand_mutations(CLI::App &app);
void generate_mutants(MutantsOptions const &opt);