#include "formulaUtility.hpp"
#include "formula.hpp"
#include "formulaCreator.hpp"
#include "ltl.hpp"
#include "state.hpp"
#include "scalarVar.hpp"
#include <algorithm>
#include <iostream>
#include <iterator> // for std::begin, std::end
#include <numeric> // for std::accumulate
#include <sstream>
#include <unordered_set>
#include <utils/stateComparer.hpp>

std::map<int, std::vector<std::shared_ptr<state>>> formulaUtility::convertToIntegerMap(const ValueStateMap & values)
{
  std::map<int, std::vector<std::shared_ptr<state>>> new_values;
  std::for_each(values.begin(), values.end(), [&new_values](const auto & value) {
    if (std::holds_alternative<int>(value.first)) {
      new_values[std::get<int>(value.first)] = value.second;
    }
  });
  return new_values;
}

std::shared_ptr<formula> formulaUtility::createEqualityFormulas(const std::string & variableName,
                                                                const ValueStateMap & include_values,
                                                                const ValueStateMap & exclude_values)
{
  std::shared_ptr<formula> res;
  if (!include_values.empty()) {
    res = formulaUtility::makeAlternativeFormula(variableName, include_values, false);
  }
  else if (!exclude_values.empty()) {
    res = formulaUtility::makeAlternativeFormula(variableName, exclude_values, true);
  }
  else {
    throw std::invalid_argument("The variable " + variableName + " has no values in the include or exclude states.");
  }
  return res;
}

std::shared_ptr<formula> formulaUtility::makeAlternativeFormula(const std::string & name, const ValueStateMap & values,
                                                                bool isInequality)
{
  std::vector<std::shared_ptr<formula>> formulas;
  formulas.reserve(values.size()); // Optional but can improve performance
  for(const auto & value : values) {
    auto formula = makeEqualityFormula(name, value.first, isInequality);
    formulas.push_back(formula);
  }
  auto combined_formula = combineFormulas(formulas, CombinationOperatorType::OR_Symbol);
  if (formulas.size() > 1) {
    combined_formula = std::make_shared<ParenthesisFormula>(combined_formula);
  }
  return combined_formula;
}

std::shared_ptr<formula> formulaUtility::makeRangeFormula(const std::string & name, int smallestValue, int largestValue)
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

std::shared_ptr<formula> formulaUtility::makeEqualityFormula(const std::string & name, ValueType value, bool isInequality)
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
    auto formula = std::make_shared<EqualsFormula>(formulaVar, valueFormula);
    return formula;
  }
}

std::pair<int, int> formulaUtility::extremeValues(const ValueStateMap & values)
{
  auto numericValues = convertToIntegerMap(values);
  auto pair = std::minmax_element(numericValues.begin(), numericValues.end(),
                                  [](const auto & a, const auto & b) { return a.first < b.first; });
  return std::make_pair(pair.first->first, pair.second->first);
}

overlapResult formulaUtility::values_overlap(const ValueStateMap & values1, const ValueStateMap & values2)
{
  auto [smallestValue_range1, largestValue_range1] = extremeValues(values1);
  auto [smallestValue_range2, largestValue_range2] = extremeValues(values2);
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

ValueType formulaUtility::getValueOfVariable(const state *  state, const std::string & name)
{
  auto scalar = state->get<scalarInt*>(name);
  assert(scalar);
  return scalar->getIntValue();
}

bool formulaUtility::isBooleanConstantWithValue(const std::shared_ptr<formula> & formula, bool value)
{
  auto boolean_formula = std::dynamic_pointer_cast<BooleanConstant>(formula);
  if (boolean_formula) {
    return boolean_formula->isTrue() == value;
  }
  return false;
}

/*
std::shared_ptr<formula> formulaUtility::combineFormulas(const std::vector<std::shared_ptr<formula>> & formulas,
                                                         const CombinationOperatorType operatorSymbol)
{
  if (formulas.empty()) {
    std::cout << "The vector of formulas is empty. I am returning a true formula." << std::endl;
    return std::make_shared<BooleanConstant>(true);
  }
  auto formula_set = StateComparer::removeDuplicates(formulas);
  auto res_formula = formula_set.front();
  if (formula_set.size() == 1) {
    return res_formula;
  }

  switch (operatorSymbol) {
  case CombinationOperatorType::AND_Symbol:
    for (size_t i = 1; i < formula_set.size(); i++) {
      auto current_formula = formula_set.at(i);
      if (isBooleanConstantWithValue(current_formula, true)) {
        continue;
      }
      res_formula = std::make_shared<AndFormula>(res_formula, current_formula);
    }
    return res_formula;
  case CombinationOperatorType::OR_Symbol:
    for (size_t i = 1; i < formula_set.size(); i++) {
      auto current_formula = formula_set.at(i);
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
*/