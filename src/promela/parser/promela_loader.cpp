#include "promela_loader.hpp"
#include <assert.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

promela_loader::promela_loader(std::string file_name, const TVL * tvl)
    : globalSymTab(nullptr), program(nullptr), automata(nullptr)
{
  // The variable promelaFile should have the fileExtension .pml
  if (file_name.find(".pml") == std::string::npos) {
    //std::cerr << "The model file must have the extension .pml." << std::endl;
    //exit(1);
    std::ofstream tempFile("_temp.pml");
    tempFile << file_name;
    file_name = "_temp.pml";
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

  if (system("cpp __workingfile.tmp __workingfile.tmp.cpp") != 0) {
    std::cerr << "Could not run the c preprocessor (cpp)." << std::endl;
    exit(1);
  }

  // Read the original file
  auto fileStream = std::make_shared<std::ifstream>(sourcePath);
  if (!fileStream->is_open()) {
    std::cerr << "The fPromela file does not exist or is not readable!" << std::endl;
    exit(1);
  }
  std::stringstream buffer;
  buffer << fileStream->rdbuf();

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

  while (globalSymTab->prevSymTab())
    globalSymTab = globalSymTab->prevSymTab();

  // Create the converter
  std::unique_ptr<ASTtoFSM> converter = std::make_unique<ASTtoFSM>();
  // Create the automata from the AST
  automata = std::make_shared<fsm>(*converter->astToFsm(globalSymTab, program, tvl));

  std::ofstream graph;
  graph.open("fsm_graphvis");
  automata->printGraphVis(graph);
  graph.close();
}


extern std::string nameSpace;
extern symbol::Type declType;
extern tdefSymNode* typeDef;
extern mtypedefSymNode* mtypeDef;

extern symTable* currentSymTab;
extern symTable* savedSymTab;

extern std::list<varSymNode*> declSyms;
extern std::list<varSymNode*> typeLst;
extern std::list<std::string> params;
extern std::list<variantQuantifier*> variants;

extern std::map<std::string, stmntLabel*> labelsMap;

extern int mtypeId;
extern bool inInline;

promela_loader::~promela_loader(){
  nameSpace = "global";
  declType = symbol::T_NA;
  typeDef = nullptr;
  mtypeDef = nullptr;

  currentSymTab = nullptr;
  savedSymTab = nullptr;

  declSyms.clear();
  typeLst.clear();
  params.clear();
  variants.clear();

  labelsMap.clear();

  mtypeId = 1;
  inInline = false;

  if(yyin != nullptr) {
		fclose(yyin);
		yylex_destroy();
	}
}