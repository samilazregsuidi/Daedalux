#include <gtest/gtest.h>
#include <memory>
// #include <gmock/gmock.h>
#include "../src/cli/promela_loader.cpp"

// Define a fixture for the tests
class PromelaLoaderTest : public ::testing::Test {
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

// // Test case for loading a valid Promela file
// TEST_F(PromelaLoaderTest, LoadValidPromelaFile) {
//     promela_loader loader;
//     std::string file_name = "minepump.pml";
//     const TVL* tvl = nullptr;

//     // Mock the necessary functions and files
//     EXPECT_CALL(fs, copy(testing::_, testing::_, testing::_)).WillOnce(testing::Return());
//     EXPECT_CALL(fs, path(testing::_)).WillOnce(testing::Return(fs::path("__workingfile.tmp")));
//     EXPECT_CALL(fs, path("__workingfile.tmp.cpp")).WillOnce(testing::Return(fs::path("__workingfile.tmp.cpp")));
//     EXPECT_CALL(fs, filesystem_error(testing::_)).WillOnce(testing::Return(fs::filesystem_error("Error")));
//     EXPECT_CALL(fs, copy_options::overwrite_existing).WillOnce(testing::Return(fs::copy_options::overwrite_existing));
//     EXPECT_CALL(fs, filesystem_error::what()).WillOnce(testing::Return("The fPromela file does not exist or is not readable!"));
//     EXPECT_CALL(fs, system("cpp __workingfile.tmp __workingfile.tmp.cpp")).WillOnce(testing::Return(0));
//     EXPECT_CALL(fs, fopen("__workingfile.tmp.cpp", "r")).WillOnce(testing::Return(yyin));
//     EXPECT_CALL(fs, fclose(yyin)).WillOnce(testing::Return(0));
//     EXPECT_CALL(fs, yylex_destroy()).WillOnce(testing::Return());
//     EXPECT_CALL(fs, ASTtoFSM()).WillOnce(testing::Return(converter));
//     EXPECT_CALL(fs, astToFsm(testing::_, testing::_, testing::_)).WillOnce(testing::Return(automata));

//     // Call the function under test
//     fsm* result = loader.load_promela_file(file_name, tvl);

//     // Assert the result
//     EXPECT_EQ(result, automata);
// }

// Test case for loading an invalid Promela file
TEST_F(PromelaLoaderTest, LoadInvalidPromelaFile) {
    // Load promela file
    std::string file_name = "invalid_file.txt";
    const TVL* tvl = nullptr;

    // Call the function under test and expect it to exit with code 1
    EXPECT_EXIT(loader->load_promela_file(file_name, tvl), ::testing::ExitedWithCode(1), "The model file must have the extension .pml.");
}

// Test case for loading an invalid Promela file
TEST_F(PromelaLoaderTest, LoadValidPromelaFile) {

    std::string current_directory = fs::current_path();
    std::string file_name = "/models/windows/original.pml";
    std::string file_path = current_directory + file_name;
    const TVL* tvl = nullptr;

    auto automata = loader->load_promela_file(file_path, tvl);

    // logging::log(logging::INFO, "Automata loaded from Promela file:");

    // EXPECT_EQ(automata->getNodes().size(), 7);
    // EXPECT_EQ(automata->getTransitions().size(), 8);
    // EXPECT_EQ(automata->getInitNodes(), automata->getNodes()[0]);
    // EXPECT_EQ(automata->getNodes()[0]->getEdges().size(), 2);
    // EXPECT_EQ(automata->getNodes()[1]->getEdges().size(), 2);
}