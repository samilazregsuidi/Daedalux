#include <gtest/gtest.h>
#include "../../src/promela/symbol/logic/temporalSymNode.hpp"

class SymNodeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup code for all tests goes here
    }

    void TearDown() override {
        // Common teardown code for all tests goes here
    }
};

// TEST_F(SymNodeTest, LTLToString) {
//     expr* ltlFormula = new SomeFormula();  // Replace SomeFormula with your actual formula type
//     ltlSymNode node("ltlName", ltlFormula, 1);

//     std::string expected = "ltl ltlName {\n\tSomeFormula\n}";
//     std::string actual = static_cast<std::string>(node);

//     EXPECT_EQ(expected, actual);
// }

// TEST_F(SymNodeTest, BLTLToString) {
//     expr* bltlFormula = new YourFormulaType();  // Replace YourFormulaType with your actual formula type
//     bltlSymNode node("bltlName", bltlFormula, 2);

//     std::string expected = "bltl bltlName {\n\tYourFormulaType\n}";
//     std::string actual = static_cast<std::string>(node);

//     EXPECT_EQ(expected, actual);
// }

// TEST_F(SymNodeTest, FMultiLTLToString) {
//     std::list<variantQuantifier*> variants;
//     variants.push_back(new SomeVariant());  // Replace SomeVariant with your actual variant type
//     variants.push_back(new AnotherVariant());  // Replace AnotherVariant with your actual variant type
//     expr* fMultiLTLFormula = new SomeFormula();  // Replace SomeFormula with your actual formula type
//     fMultiLTLSymNode node("fMultiLTLName", variants, fMultiLTLFormula, 3);

//     std::string expected = "fMultiLTL fMultiLTLName SomeVariant, AnotherVariant {\n\tSomeFormula\n}";
//     std::string actual = static_cast<std::string>(node);

//     EXPECT_EQ(expected, actual);
// }