#include "promela_loader.hpp"

namespace fs = std::filesystem;

void load_promela_file(std::string file_name, symTable *&globalSymTab, stmnt *&program){
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
        std::cout << "File copied successfully." << std::endl;
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
	
	if (yyparse(&globalSymTab, &program) != 0)
	{
		std::cerr << "Syntax error; aborting." << std::endl;
		exit(1);
	}

	if (yyin != nullptr)
	{
		fclose(yyin);
		yylex_destroy();
	}
}