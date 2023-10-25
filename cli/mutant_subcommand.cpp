#include "mutant_subcommand.hpp"

void setup_subcommand_mutations(CLI::App &app)
{

	// Create the option and subcommand objects.
	auto opt = std::make_shared<MutantsOptions>();
	auto sub = app.add_subcommand("mutants", "Generate mutants from a Promela file");

	app.add_option("-f, --file", opt->input_file, "Promela file to mutate")->check(CLI::ExistingFile)->required();
	app.add_option("-p, --property", opt->property_file, "Property file to mutate")->check(CLI::ExistingFile)->required();
	app.add_option("-n, --nb_mutants", opt->number_of_mutants, "Number of mutants to generate")->check(CLI::PositiveNumber)->required();

	// Set the run function as callback to be called when this subcommand is issued.
	sub->callback([opt]()
				  { generate_mutants(*opt); });
}

void generate_mutants(MutantsOptions const &opt)
{
	std::cout << "Generating " << opt.number_of_mutants << " mutants from " << opt.input_file << " and " << opt.property_file << std::endl;

	// Load promela file
	promela_loader *loader = new promela_loader();
	loader->load_promela_file(opt.input_file, nullptr);
	stmnt *program = loader->get_program();

	unsigned int index = program->assignMutables();
	std::cout << "NUMBER OF MUTABLE NODES " << index << std::endl;

	// Folder of the original program
	std::string folder = opt.input_file.substr(0, opt.input_file.find_last_of("/"));
	std::string mutant_folder = folder + "/mutants";
	std::string property_file_name = opt.property_file.substr(opt.input_file.find_last_of("/") + 1);

	// Write original program to file so that it can be used by the mutation operators
	// Create folder for mutants if it does not exist
	if (!std::filesystem::exists(mutant_folder))
	{
		std::filesystem::create_directory(mutant_folder);
		std::cout << "Created folder " << mutant_folder << std::endl;
	}

	std::ofstream output;
	output.open(mutant_folder + "/original.pml");
	output << "#include \"../" << property_file_name << "\"" << std::endl
		   << std::endl;
	output << stmnt::string(program);
	output.close();

	// Generate mutants
	for (unsigned int j = 0; j < opt.number_of_mutants; j++)
	{
		for (unsigned int i = 1; i <= index; i++)
		{
			// Generate mutant by mutating the original program and writing it to a file
			auto copy = program->deepCopy();
			astNode::mutate(copy, i);
			std::string file_name = mutant_folder + "/mutant_" + std::to_string(j * index + i) + ".pml";
			output.open(file_name);
			// Write property file to mutant folder as well
			output << "#include \"../" << property_file_name << "\"" << std::endl
				   << std::endl;
			output << stmnt::string(copy);
			output.close();
			delete copy;
		}
	}

	std::cout << "Generated " << opt.number_of_mutants * index << " mutants" << std::endl;
	delete loader;
}