#include "promela_loader.hpp"
#include <assert.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

promela_loader::promela_loader(std::string file_name, const TVL * tvl)
    : automata(nullptr), globalSymTab(nullptr), program(nullptr)
{
  printf("Running the promela loader\n");
  // The variable promelaFile should have the fileExtension .pml
  if (file_name.find(".pml") == std::string::npos) {
    std::cerr << "The model file must have the extension .pml." << std::endl;
    exit(1);
  }
  // Copy the model file to a temporary file
  fs::path sourcePath = file_name;
  std::string current_directory = fs::current_path();
  auto destinationFile = current_directory + "/__workingfile.tmp";
  fs::path destinationPath = destinationFile;
  try {
    fs::copy(sourcePath, destinationPath, fs::copy_options::overwrite_existing);
  }
  catch (const fs::filesystem_error & e) {
    std::cerr << "Error: " << e.what() << std::endl;
    std::cerr << "The fPromela file does not exist or is not readable!" << std::endl;
    exit(1);
  }
  printf("Running the c preprocessor (cpp) on the file: %s\n", file_name.c_str());

  if (system("cpp __workingfile.tmp __workingfile.tmp.cpp") != 0) {
    std::cerr << "Could not run the c preprocessor (cpp)." << std::endl;
    exit(1);
  }

  printf("Loading file: %s\n", file_name.c_str());

  // Read the original file
  auto fileStream = std::make_shared<std::ifstream>(sourcePath);
  if (!fileStream->is_open()) {
    std::cerr << "The fPromela file does not exist or is not readable!" << std::endl;
    exit(1);
  }
  std::stringstream buffer;
  buffer << fileStream->rdbuf();

  printf("Parsing the file\n");

  // Open the temporary file
  yyin = fopen("__workingfile.tmp.cpp", "r");
  if (yyin == nullptr) {
    std::cerr << "Could not open temporary working file (" << file_name << ")." << std::endl;
    exit(1);
  }
  init_lex();

  if (yyparse(&this->globalSymTab, &this->program) != 0) {
    std::cerr << "Syntax error; aborting." << std::endl;
    exit(1);
  }

  if (yyin != nullptr) {
    fclose(yyin);
    yylex_destroy();
  }

  printf("Creating the AST\n");

  while (globalSymTab->prevSymTab())
    globalSymTab = globalSymTab->prevSymTab();

  // Create the converter
  std::unique_ptr<ASTtoFSM> converter = std::make_unique<ASTtoFSM>();
  // Create the automata from the AST
  printf("Creating the automata from the AST\n");
  automata = std::make_shared<fsm>(*converter->astToFsm(globalSymTab, program, tvl));

  // Note: The following code is commented out because it is not used in the current implementation.
  //   if (buffer.str() != stmnt::string(program)) {
  //     std::cerr << "The program is not equal to the original program." << std::endl;
  //     std::cerr << "The program is: " << std::endl;
  //     std::cerr << stmnt::string(program) << std::endl;
  //     std::cerr << "The original program is: " << std::endl;
  //     std::cerr << buffer.str() << std::endl;
  //     exit(1);
  //   }

  printf("Printing graphvis\n");

  std::ofstream graph;
  graph.open("fsm_graphvis");
  automata->printGraphVis(graph);
  graph.close();
}