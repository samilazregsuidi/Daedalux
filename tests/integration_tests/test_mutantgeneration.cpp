#include <gtest/gtest.h>
#include <filesystem>
#include <memory>
#include "../../src/promela/parser/promela_loader.hpp"
#include "../../src/core/ast/stmnt/stmnt.hpp"
#include "../../src/algorithm/explore.hpp"



// Define a fixture for the tests
class DISABLED_MutantGenerationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup code that will be called before each test
    }

    void TearDown() override {
        // Common teardown code that will be called after each test
    }
};


TEST_F(DISABLED_MutantGenerationTest, GenerateMutant) {
    std::string current_directory = std::filesystem::current_path();
    std::string file_name = "/models/windows/original.pml";
    std::string file_path = current_directory + file_name;
    const TVL* tvl = nullptr;
    // auto loader = std::make_unique<promela_loader>(file_path, tvl);
    // auto original = loader->getAutomata().get();

    // stmnt* program = loader->getProgram();
    // std::cout << "Original program" << std::endl;
    // std::cout << stmnt::string(program) << std::endl;


    // unsigned int index = program->assignMutables();

    // std::cout << "NUMBER OF MUTABLE NODES " << index << std::endl;

    // // Folder of the original program
    // std::string mutant_folder = current_directory + "/mutants";

    // // Write original program to file so that it can be used by the mutation operators
    // // Create folder for mutants if it does not exist
    // if (!std::filesystem::exists(mutant_folder)) {
    //     std::filesystem::create_directory(mutan t_folder);
    // }

    // std::cout << "Created folder " << mutant_folder << std::endl;
    // std::ofstream output;
    // output.open(mutant_folder + "/original.pml");
    // output << stmnt::string(program);
    // output.close();

    // Generate mutants
    // auto copy = program->deepCopy();
    // astNode::mutate(copy, 10);
    // assert(stmnt::string(copy) != stmnt::string(program));
    // std::ofstream output_mutant;
    // std::string mutant_file = mutant_folder + "/mutant.pml";
    // output_mutant.open(mutant_file);
    // output_mutant << stmnt::string(copy);
    // output_mutant.close();
    // delete copy;

    // // Load promela files
    // auto loader_mutant = std::make_unique<promela_loader>(mutant_file, tvl);
    // auto fsm_mutant = loader_mutant->getAutomata();

    // auto loader_original = std::make_unique<promela_loader>(file_path, tvl);    
    // auto fsm_original = loader_original->getAutomata();

    // std::cout << "Ready to generate traces" << std::endl;

    // // Generate traces
    // auto report = generateTraces(fsm_original, fsm_mutant);

    // // // Assert that the traces are not empty
    // ASSERT_FALSE(report->getGoodTraces().empty());
    // ASSERT_FALSE(report->getBadTraces().empty());
}