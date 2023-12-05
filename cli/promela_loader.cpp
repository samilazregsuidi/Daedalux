#include "promela_loader.hpp"

namespace fs = std::filesystem;

promela_loader::promela_loader(std::string file_name, const TVL *tvl)
	: automata(nullptr)
	, globalSymTab(nullptr)
	, program(nullptr)
{
	// The variable promelaFile should have the fileExtension .pml
	if (file_name.find(".pml") == std::string::npos){
		std::cerr << "The model file must have the extension .pml." << std::endl;
		exit(1);
	}
	// Copy the model file to a temporary file
	fs::path sourcePath = file_name;
	fs::path destinationPath = "__workingfile.tmp";

	try {
        fs::copy(sourcePath, destinationPath, fs::copy_options::overwrite_existing);
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "The fPromela file does not exist or is not readable!" << std::endl;
		exit(1);
    }

	if (system("cpp __workingfile.tmp __workingfile.tmp.cpp") != 0)
	{
		std::cerr << "Could not run the c preprocessor (cpp)." << std::endl;
		exit(1);
	}

	yyin = fopen("__workingfile.tmp.cpp", "r");
	if (yyin == nullptr)
	{
		std::cerr << "Could not open temporary working file (" << file_name << ")." << std::endl;
		exit(1);
	}
	init_lex();

	if (yyparse(&this->globalSymTab, &this->program) != 0)
	{
		std::cerr << "Syntax error; aborting." << std::endl;
		exit(1);
	}

	if (yyin != nullptr)
	{
		fclose(yyin);
		yylex_destroy();
	}

	
	while(globalSymTab->prevSymTab()) 
		globalSymTab = globalSymTab->prevSymTab();

	ASTtoFSM *converter = new ASTtoFSM();
	// Create the automata from the AST
	automata = converter->astToFsm(globalSymTab, program, tvl);

	

	std::ofstream graph;
	graph.open("fsm_graphvis");
	automata->printGraphVis(graph);
	graph.close();
}