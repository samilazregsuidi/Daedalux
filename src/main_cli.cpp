#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "CLI11.hpp"
#include "modelchecking_subcommand.hpp"
#include "mutant_subcommand.hpp"

/**
 * Make some basic checks on the architecture.
 */
bool check_architecture(){
	// Some basic validity checks
	if (sizeof(int) != 4)
	{
		std::cout << "Bad architecture: int type must be four bytes long." << std::endl;
		return false;
	}
	if (sizeof(short) != 2)
	{
		std::cout << "Bad architecture: short type must be two bytes long." << std::endl;
		return false;
	}
	if (sizeof(unsigned long) != 8)
	{
		std::cout << "Bad architecture: long type must be two bytes long.\n";
		return false;
	}
	if (sizeof(double) != 8)
	{
		std::cout << "Bad architecture: double type must be two bytes long.\n";
		return false;
	}
	if (sizeof(void *) != 8)
	{
		std::cout << "Bad architecture: pointer type must be eight bytes long.\n";
		return false;
	}
	return true;
}

int main(int argc, char *argv[])
{
	// Check architecture - exit if not supported
	if(!check_architecture()) {
		exit(1);
	}

	CLI::App app;

	
	/*
	std::string ltlProp;
	std::string path;

	model.add_option("-ltl", ltlProp, "LTL property to check.");
	if (!ltlProp.empty())
	{
		std::string errorMsg;
		if (!appendClaim("__workingfile.tmp", path, ltlProp, errorMsg))
			error("The LTL formula '%s' could not be parsed, error message: \n --\n%s\n", ltlProp, errorMsg);
	}
	*/

	/**********************************************************************************************************************************/
    setup_subcommand_mutations(app);
	setup_subcommand_modelchecking(app);	

	std::cout << "Setting up subcommands" << std::endl;

	app.footer("Daedalux 2024 - University of Luxembourg");

	// app.require_subcommand();
	CLI11_PARSE(app, argc, argv);

	exit(0);
}