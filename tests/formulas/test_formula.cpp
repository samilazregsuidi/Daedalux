#include "../../src/core/semantic/variable/state/state.hpp"
#include "../../src/formulas/formula.hpp"
#include "../../src/formulas/formulaCreator.hpp"

#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

// Define a fixture for the tests
class FormulaTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {}

  // Function that checks if a string starts with a given prefix
  bool startsWith(const std::string & str, const std::string & prefix)
  {
    if (str.length() < prefix.length()) {
      return false;
    }
    return str.substr(0, prefix.length()) == prefix;
  }
};

TEST_F(FormulaTest, defNameOfLargerEquals)
{
  auto var_x = std::make_shared<VariableFormula>("x");
  auto var_y = std::make_shared<VariableFormula>("y");
  auto expected_def_name = "x_larger_equals_y";
  auto formula = LargerEqualsFormula(var_x, var_y);
  auto formula_string = formula.toFormula();
  ASSERT_EQ(formula_string, "x >= y");
  auto definition = formula.getDefinitionString();
  auto expected_definition = "#define x_larger_equals_y (x >= y)\n";
  ASSERT_EQ(definition, expected_definition);
  auto def_name = formula.getDefName();
  ASSERT_EQ(def_name, expected_def_name);

  auto promela_specification = formula.promelaFormula();
  auto expected_promela_specification = expected_def_name;
  ASSERT_EQ(promela_specification, expected_promela_specification);
}

TEST_F(FormulaTest, defNameOfComplexFormula)
{
  auto array_1 = std::make_shared<VariableFormula>("array[1]");
  auto array_2 = std::make_shared<VariableFormula>("array[2]");
  auto array_3 = std::make_shared<VariableFormula>("array[3]");
  auto i = std::make_shared<VariableFormula>("i");
  auto formula_1 =
      std::make_shared<AndFormula>(std::make_shared<LargerEqualsFormula>(array_1, std::make_shared<NumberConstant>(0)),
                                   std::make_shared<SmallerEqualsFormula>(array_1, std::make_shared<NumberConstant>(1)));
  auto formula_2 = std::make_shared<OrFormula>(std::make_shared<EqualsFormula>(array_2, std::make_shared<NumberConstant>(0)),
                                               std::make_shared<EqualsFormula>(array_2, std::make_shared<NumberConstant>(2)));
  auto formula_3 = std::make_shared<OrFormula>(std::make_shared<EqualsFormula>(array_3, std::make_shared<NumberConstant>(0)),
                                               std::make_shared<EqualsFormula>(array_3, std::make_shared<NumberConstant>(3)));

  auto equal_1 = std::make_shared<EqualsFormula>(i, std::make_shared<NumberConstant>(0));
  auto equal_2 = std::make_shared<EqualsFormula>(i, std::make_shared<NumberConstant>(1));
  auto equal_3 = std::make_shared<EqualsFormula>(i, std::make_shared<NumberConstant>(3));
  auto equal_4 = std::make_shared<EqualsFormula>(i, std::make_shared<NumberConstant>(4));
  std::vector<std::shared_ptr<formula>> equal_formulas = {equal_1, equal_2, equal_3, equal_4};
  auto formula4 = formulaUtility::combineFormulas(equal_formulas, CombinationOperatorType::OR_Symbol);
  auto formula1_parenthesis = std::make_shared<ParenthesisFormula>(formula_1);
  auto formula2_parenthesis = std::make_shared<ParenthesisFormula>(formula_2);
  auto formula3_parenthesis = std::make_shared<ParenthesisFormula>(formula_3);
  auto formula4_parenthesis = std::make_shared<ParenthesisFormula>(formula4);
  std::vector<std::shared_ptr<formula>> formulas = {formula1_parenthesis, formula2_parenthesis, formula3_parenthesis,
                                                    formula4_parenthesis};
  auto formula = formulaUtility::combineFormulas(formulas, CombinationOperatorType::AND_Symbol);
  auto formula_string = formula->toFormula();
  auto expected_formula_string = "(array[1] >= 0 && array[1] <= 1) && (array[2] == 0 || array[2] == 2) && (array[3] == 0 || "
                                 "array[3] == 3) && (i == 0 || i == 1 || i == 3 || i == 4)";
  ASSERT_EQ(formula_string, expected_formula_string);
  auto definition = formula->getDefinitionString();
  auto expected_definition =
      "#define array_1__larger_equals_0 (array[1] >= 0)\n#define array_1__smaller_equals_1 (array[1] <= 1)\n#define "
      "array_2__equals_0 (array[2] == 0)\n#define array_2__equals_2 (array[2] == 2)\n#define array_3__equals_0 (array[3] == "
      "0)\n#define array_3__equals_3 (array[3] == 3)\n#define i_equals_0 (i == 0)\n#define i_equals_1 (i == 1)\n#define "
      "i_equals_3 (i == 3)\n#define i_equals_4 (i == 4)\n";
  ASSERT_EQ(definition, expected_definition);
  auto def_name = formula->getDefName();
  auto expected_def_name = "array_1__larger_equals_0\narray_1__smaller_equals_1\narray_2__equals_0\narray_2__equals_2\narray_3_"
                           "_equals_0\narray_3__equals_3\ni_equals_0\ni_equals_1\ni_equals_3\ni_equals_4";
  ASSERT_EQ(def_name, expected_def_name);
  auto promela_specification = formula->promelaFormula();
  auto expected_promela_specification =
      "(array_1__larger_equals_0 && array_1__smaller_equals_1) && (array_2__equals_0 || array_2__equals_2) && "
      "(array_3__equals_0 || array_3__equals_3) && (i_equals_0 || i_equals_1 || i_equals_3 || i_equals_4)";
  ASSERT_EQ(promela_specification, expected_promela_specification);
}

TEST_F(FormulaTest, neverClaimOfComparisonFormula)
{
  auto var_x = std::make_shared<VariableFormula>("x");
  auto var_y = std::make_shared<VariableFormula>("y");
  auto formula = LargerEqualsFormula(var_x, var_y);
  auto neverClaim = formula.neverClaim();
  EXPECT_TRUE(startsWith(neverClaim, "never"));
}

TEST_F(FormulaTest, neverClaimOfGloballyFormula)
{
  auto var_x = std::make_shared<VariableFormula>("x");
  auto var_y = std::make_shared<VariableFormula>("y");
  auto innerFormula = std::make_shared<LargerEqualsFormula>(var_x, var_y);
  auto formula = std::make_shared<GloballyFormula>(innerFormula);
  auto neverClaim = formula->neverClaim();
  EXPECT_TRUE(startsWith(neverClaim, "never"));
}

TEST_F(FormulaTest, neverClaimOfNegatedFormula)
{
  auto var_x = std::make_shared<VariableFormula>("x");
  auto var_y = std::make_shared<VariableFormula>("y");
  auto innerFormula = std::make_shared<LargerEqualsFormula>(var_x, var_y);
  auto formula = std::make_shared<NotFormula>(innerFormula);
  auto neverClaim = formula->neverClaim();
  EXPECT_TRUE(startsWith(neverClaim, "never"));
}

TEST_F(FormulaTest, neverClaimOfFinallyFormula)
{
  auto var_x = std::make_shared<VariableFormula>("x");
  auto var_y = std::make_shared<VariableFormula>("y");
  auto innerFormula = std::make_shared<LargerEqualsFormula>(var_x, var_y);
  auto formula = std::make_shared<FinallyFormula>(innerFormula);
  auto neverClaim = formula->neverClaim();
  EXPECT_TRUE(startsWith(neverClaim, "never"));
}

TEST_F(FormulaTest, neverClaimOfComplexFormula)
{
  auto array_1 = std::make_shared<VariableFormula>("array[1]");
  auto array_2 = std::make_shared<VariableFormula>("array[2]");
  auto array_3 = std::make_shared<VariableFormula>("array[3]");
  auto i = std::make_shared<VariableFormula>("i");
  auto formula_1 =
      std::make_shared<AndFormula>(std::make_shared<LargerEqualsFormula>(array_1, std::make_shared<NumberConstant>(0)),
                                   std::make_shared<SmallerEqualsFormula>(array_1, std::make_shared<NumberConstant>(1)));
  auto formula_2 = std::make_shared<OrFormula>(std::make_shared<EqualsFormula>(array_2, std::make_shared<NumberConstant>(0)),
                                               std::make_shared<EqualsFormula>(array_2, std::make_shared<NumberConstant>(2)));
  auto formula_3 = std::make_shared<OrFormula>(std::make_shared<EqualsFormula>(array_3, std::make_shared<NumberConstant>(0)),
                                               std::make_shared<EqualsFormula>(array_3, std::make_shared<NumberConstant>(3)));

  auto equal_1 = std::make_shared<EqualsFormula>(i, std::make_shared<NumberConstant>(0));
  auto equal_2 = std::make_shared<EqualsFormula>(i, std::make_shared<NumberConstant>(1));
  auto equal_3 = std::make_shared<EqualsFormula>(i, std::make_shared<NumberConstant>(3));
  auto equal_4 = std::make_shared<EqualsFormula>(i, std::make_shared<NumberConstant>(4));
  std::vector<std::shared_ptr<formula>> equal_formulas = {equal_1, equal_2, equal_3, equal_4};
  auto formula4 = formulaUtility::combineFormulas(equal_formulas, CombinationOperatorType::OR_Symbol);
  auto formula1_parenthesis = std::make_shared<ParenthesisFormula>(formula_1);
  auto formula2_parenthesis = std::make_shared<ParenthesisFormula>(formula_2);
  auto formula3_parenthesis = std::make_shared<ParenthesisFormula>(formula_3);
  auto formula4_parenthesis = std::make_shared<ParenthesisFormula>(formula4);
  std::vector<std::shared_ptr<formula>> formulas = {formula1_parenthesis, formula2_parenthesis, formula3_parenthesis,
                                                    formula4_parenthesis};
  auto formula = formulaUtility::combineFormulas(formulas, CombinationOperatorType::AND_Symbol);
  auto neverClaim = formula->neverClaim();
  EXPECT_TRUE(startsWith(neverClaim, "never"));
}