#include "formula.hpp"
#include "formulaCreator.hpp"
#include "ltl.hpp"
#include "primitiveVariable.hpp"
#include "state.hpp"
#include "uTypeVariable.hpp"
#include <algorithm>
#include <iostream>
#include <iterator> // for std::begin, std::end
#include <mtypeVariable.hpp>
#include <numeric> // for std::accumulate
#include <sstream>

std::map<int, std::vector<std::shared_ptr<state>>>
formulaUtility::convertToIntegerMap(const std::map<ValueType, std::vector<std::shared_ptr<state>>> & values)
{
  std::map<int, std::vector<std::shared_ptr<state>>> new_values;
  for (auto value : values) {
    if (std::holds_alternative<int>(value.first)) {
      new_values[std::get<int>(value.first)] = value.second;
    }
    else {
      std::cout << "The value is not an integer. I am skipping it." << std::endl;
    }
  }
  return new_values;
}

std::shared_ptr<formula>
formulaUtility::makeAlternativeFormula(std::string name, const std::map<ValueType, std::vector<std::shared_ptr<state>>> values,
                                       bool isInequality)
{
  std::vector<std::shared_ptr<formula>> formulas;
  for (auto value : values) {
    auto formula = makeEqualityFormula(name, value.first, isInequality);
    formulas.push_back(formula);
  }
  auto combined_formula = combineFormulas(formulas, CombinationOperatorType::OR_Symbol);
  if (values.size() > 1) {
    combined_formula = std::make_shared<ParenthesisFormula>(combined_formula);
  }
  return combined_formula;
}

std::shared_ptr<formula> formulaUtility::makeRangeFormula(std::string name, int smallestValue, int largestValue)
{
  assert(smallestValue <= largestValue);
  auto formulaVar = std::make_shared<VariableFormula>(name);
  auto largestValueFormula = std::make_shared<NumberConstant>(largestValue);
  auto smallestValueFormula = std::make_shared<NumberConstant>(smallestValue);
  auto largerThanFormula = std::make_shared<LargerEqualsFormula>(formulaVar, smallestValueFormula);
  auto smallerThanFormula = std::make_shared<SmallerEqualsFormula>(formulaVar, largestValueFormula);
  auto local_formula = std::make_shared<AndFormula>(largerThanFormula, smallerThanFormula);
  auto parent_formula = std::make_shared<ParenthesisFormula>(local_formula);
  return parent_formula;
}

std::shared_ptr<formula> formulaUtility::makeEqualityFormula(std::string name, ValueType value, bool isInequality)
{
  auto formulaVar = std::make_shared<VariableFormula>(name);
  std::shared_ptr<LeafFormula> valueFormula;
  if (std::holds_alternative<int>(value)) {
    valueFormula = std::make_shared<NumberConstant>(std::get<int>(value));
  }
  else if (std::holds_alternative<std::string>(value)) {
    valueFormula = std::make_shared<VariableFormula>(std::get<std::string>(value));
  }
  else if (std::holds_alternative<bool>(value)) {
    valueFormula = std::make_shared<BooleanConstant>(std::get<bool>(value));
  }
  else {
    throw std::invalid_argument("The value of the variable " + name + " is not a boolean, integer, or string.");
  }
  if (isInequality) {
    return std::make_shared<NotEqualsFormula>(formulaVar, valueFormula);
  }
  else {
    return std::make_shared<EqualsFormula>(formulaVar, valueFormula);
  }
}

int formulaUtility::largestValue(const std::map<ValueType, std::vector<std::shared_ptr<state>>> & values)
{
  auto numericValues = convertToIntegerMap(values);
  auto largestElement = std::max_element(numericValues.begin(), numericValues.end(),
                                         [](const auto & a, const auto & b) { return a.first < b.first; });
  return largestElement->first;
}

int formulaUtility::smallestValue(const std::map<ValueType, std::vector<std::shared_ptr<state>>> & values)
{
  auto numericValues = convertToIntegerMap(values);
  auto smallestElement = std::min_element(numericValues.begin(), numericValues.end(),
                                          [](const auto & a, const auto & b) { return a.first < b.first; });
  return smallestElement->first;
}

overlapResult formulaUtility::values_overlap(const std::map<ValueType, std::vector<std::shared_ptr<state>>> & values1,
                                             const std::map<ValueType, std::vector<std::shared_ptr<state>>> & values2)
{
  auto largestValue_range1 = largestValue(values1);
  auto smallestValue_range1 = smallestValue(values1);
  auto largestValue_range2 = largestValue(values2);
  auto smallestValue_range2 = smallestValue(values2);
  bool overlaps = (smallestValue_range1 <= largestValue_range2 && smallestValue_range2 <= largestValue_range1);
  overlapResult result;
  result.value_ranges_overlap = overlaps;
  if (!overlaps) {
    // The value ranges do not overlap.
    // They can be separated using a comparison formula such as larger than or smaller than.
    if (smallestValue_range1 >= largestValue_range2) {
      // The first range is larger than the second range. We can use the smallest value in the first range.
      result.boundary_value = smallestValue_range1;
      result.is_larger = true;
    }
    else {
      // The first range is smaller than the second range. We can use the largest value in the first range.
      result.boundary_value = largestValue_range1;
      result.is_larger = false;
    }
  }
  return result;
}

ValueType formulaUtility::getValueOfVariable(const std::shared_ptr<state> & state, const std::string & name)
{
  auto variable = state->getVariable(name);
  primitiveVariable * casted_variable = nullptr;
  mtypeVar * casted_variable_mType = nullptr;
  switch (variable->getType()) {
  case variable::V_BOOL:
    casted_variable = dynamic_cast<primitiveVariable *>(variable);
    assert(casted_variable);
    return casted_variable->getValue() == 1;
  case variable::V_BIT:
  case variable::V_BYTE:
  case variable::V_PID:
  case variable::V_CID:
  case variable::V_SHORT:
  case variable::V_INT:
    casted_variable = dynamic_cast<primitiveVariable *>(variable);
    assert(casted_variable);
    return casted_variable->getValue();
    break;
  case variable::V_MTYPE:
    casted_variable_mType = dynamic_cast<mtypeVar *>(variable);
    assert(casted_variable_mType);
    return casted_variable_mType->getValueName();
    break;
  case variable::V_CMTYPE:
    return name;
    break;
  case variable::V_UTYPE:
    std::cout << "The variable " << name << " is a user type. I am returning a string." << std::endl;
    // Need to implement
    // value = casted_variable->getValue();
    break;
  default:
    break;
  }
  return 0;
}

bool formulaUtility::isBooleanConstantWithValue(const std::shared_ptr<formula> & formula, bool value)
{
  auto boolean_formula = std::dynamic_pointer_cast<BooleanConstant>(formula);
  if (boolean_formula) {
    return boolean_formula->isTrue() == value;
  }
  return false;
}

std::shared_ptr<formula> formulaUtility::combineFormulas(const std::vector<std::shared_ptr<formula>> & formulas,
                                                         const CombinationOperatorType operatorSymbol)
{
  if (formulas.empty()) {
    std::cout << "The vector of formulas is empty. I am returning a true formula." << std::endl;
    return std::make_shared<BooleanConstant>(true);
  }
  auto res_formula = formulas.front();
  if (formulas.size() == 1) {
    return res_formula;
  }
  auto formula_set = std::set<std::shared_ptr<formula>>(formulas.begin(), formulas.end());
  if (formula_set.size() < formulas.size()) {
    std::cout << "The vector of formulas contains duplicates. I am removing them." << std::endl;
  }
  switch (operatorSymbol) {
  case CombinationOperatorType::AND_Symbol:
    for (size_t i = 1; i < formulas.size(); i++) {
      auto current_formula = formulas.at(i);
      if (isBooleanConstantWithValue(current_formula, true)) {
        continue;
      }
      res_formula = std::make_shared<AndFormula>(res_formula, current_formula);
    }
    return res_formula;
  case CombinationOperatorType::OR_Symbol:
    for (size_t i = 1; i < formulas.size(); i++) {
      auto current_formula = formulas.at(i);
      if (isBooleanConstantWithValue(current_formula, false)) {
        continue;
      }
      res_formula = std::make_shared<OrFormula>(res_formula, current_formula);
    }
    return res_formula;
  default:
    throw std::invalid_argument("Unknown operator symbol");
  }
}