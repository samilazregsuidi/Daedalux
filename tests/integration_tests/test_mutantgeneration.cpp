#include <gtest/gtest.h>
#include <filesystem>
#include <memory>
#include "../../src/cli/promela_loader.hpp"
#include "../../ast/stmnt/stmnt.hpp"


// Define a fixture for the tests
class MutantGenerationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup code that will be called before each test
        loader = std::make_unique<promela_loader>();
    }

    void TearDown() override {
        // Common teardown code that will be called after each test
    }
    std::unique_ptr<promela_loader> loader;
};


TEST_F(MutantGenerationTest, LoadValidPromelaFile) {
    std::string current_directory = fs::current_path();
    std::string file_name = "/models/windows/original.pml";
    std::string file_path = current_directory + file_name;
    const TVL* tvl = nullptr;

    auto automata = loader->load_promela_file(file_path, tvl);

    stmnt* program = loader->get_program();

    unsigned int index = program->assignMutables();

    // Folder of the original program
    std::string folder = opt.input_file.substr(0, opt.input_file.find_last_of("/"));
    std::string mutant_folder = folder + "/mutants";

    // Write original program to file so that it can be used by the mutation operators
    // Create folder for mutants if it does not exist
    if (!std::filesystem::exists(mutant_folder)) {
        std::filesystem::create_directory(mutant_folder);
    }

    std::ofstream output;
    output.open(mutant_folder + "/original.pml");
    output << stmnt::string(program);
    output.close();

    // Generate mutants
    auto copy = program->deepCopy();
    astNode::mutate(copy, 1);
    std::string mutant_file = mutant_folder + "/mutant_" + std::to_string(j) + ".pml";
    output.open(mutant_file);
    output << stmnt::string(copy);
    output.close();
    delete copy;

    // Load promela files
    promela_loader * loader_mutant = new promela_loader();
    auto fsm_mutant = loader_mutant->load_promela_file(mutant_file, nullptr);
    delete loader_mutant;
    promela_loader * loader_original = new promela_loader();
    auto fsm_original = loader_original->load_promela_file(file_path, nullptr);
    delete loader_original;

    // Generate traces
    traceReport report = generateTraces(fsm_original, fsm_mutant);

    // Assert that the traces are not empty
    ASSERT_FALSE(report.getGoodTraces().empty());
    ASSERT_FALSE(report.getBadTraces().empty());
}