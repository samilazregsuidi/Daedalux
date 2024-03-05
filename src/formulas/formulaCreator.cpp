#include "formulaCreator.hpp"
#include "formula.hpp"
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

bool sameStates(const std::vector<std::shared_ptr<state>> & states1, const std::vector<std::shared_ptr<state>> & states2)
{
  if (states1.size() != states2.size()) {
    return false;
  }
  for (size_t i = 0; i < states1.size(); i++) {
    auto state1 = states1[i];
    bool isFound = false;
    for (size_t j = 0; j < states2.size(); j++) {
      auto state2 = states2[j];
      if (state1->isSame(state2.get())) {
        isFound = true;
        break;
      }
    }
    if (!isFound) {
      return false;
    }
  }
  return true;
}

bool isConstant(const std::map<ValueType, std::vector<std::shared_ptr<state>>> & values) { return values.size() == 1; }

std::unordered_map<std::string, std::map<ValueType, std::vector<std::shared_ptr<state>>>>
formulaCreator::buildVariableValueMap(const std::vector<std::shared_ptr<state>> & states)
{
  // The vector of states must not be empty.
  assert(states.size() > 0);
  // Get all variables that are visible in the states.
  auto variables = states.front()->getAllVisibleVariables();
  std::unordered_map<std::string, std::map<ValueType, std::vector<std::shared_ptr<state>>>> variable_val_map;
  for (auto var : variables) {
    std::string name = var->getVisibleName();
    std::map<ValueType, std::vector<std::shared_ptr<state>>> value_map;
    for (auto stateVar : states) {
      auto value = formulaUtility::getValueOfVariable(stateVar, name);
      value_map[value].push_back(stateVar);
    }
    variable_val_map[name] = value_map;
  }
  return variable_val_map;
}

std::shared_ptr<formula> formulaCreator::distinguishTraces(const std::shared_ptr<trace> & include_trace,
                                                           const std::shared_ptr<trace> & exclude_trace)
{
  // Remove the common prefixes from the traces.
  auto [include_trace_trimmed, exclude_trace_trimmed] = removeCommonPrefixes(include_trace, exclude_trace);
  if (include_trace_trimmed->getStates().empty() || exclude_trace_trimmed->getStates().empty()) {
    std::cout << "The traces are the same. I am returning a false formula as they cannot be distinguished." << std::endl;
    return std::make_shared<BooleanConstant>(false);
  }
  auto state1 = include_trace_trimmed->getStates().front();
  auto state2 = exclude_trace_trimmed->getStates().front();
  assert(state1->isSame(state2.get()));
  auto successors1 = state1->Post();
  auto successors2 = state2->Post();
  auto successor1_vec = std::vector<std::shared_ptr<state>>(successors1.begin(), successors1.end());
  auto successor2_vec = std::vector<std::shared_ptr<state>>(successors2.begin(), successors2.end());
  auto successor1 = successor1_vec.front();
  auto successor2 = successor2_vec.front();
  successor1->printDelta(successor2.get());
  if (sameStates(successor1_vec, successor2_vec)) {
    std::cout << "The successors are the same. I am returning a false formula as they cannot be distinguished." << std::endl;
    return std::make_shared<BooleanConstant>(false);
  }
  auto transitionFormula = createTransitionFormula(state1, successor1_vec, successor2_vec);
  return transitionFormula;
}

std::map<ValueType, std::vector<std::shared_ptr<state>>>
remove_duplicated_values(const std::map<ValueType, std::vector<std::shared_ptr<state>>> & values1,
                         const std::map<ValueType, std::vector<std::shared_ptr<state>>> & values2)
{
  std::map<ValueType, std::vector<std::shared_ptr<state>>> new_values;
  for (auto value : values1) {
    if (values2.find(value.first) == values2.end()) {
      new_values[value.first] = value.second;
    }
  }
  return new_values;
}

std::shared_ptr<formula>
formulaCreator::createVariableFormula(std::string variableName,
                                      std::map<ValueType, std::vector<std::shared_ptr<state>>> include_values,
                                      std::map<ValueType, std::vector<std::shared_ptr<state>>> exclude_values)
{
  auto values_only_in_include = remove_duplicated_values(include_values, exclude_values);
  auto values_only_in_exclude = remove_duplicated_values(exclude_values, include_values);
  if (values_only_in_include.empty() && values_only_in_exclude.empty()) {
    // The variable has the same value in all states - It cannot be used to distinguish the states.
    return std::make_shared<BooleanConstant>(true);
  }

  auto arbitrary_value =
      values_only_in_include.empty() ? values_only_in_exclude.begin()->first : values_only_in_include.begin()->first;

  if (std::holds_alternative<bool>(arbitrary_value)) {
    // We can only have one value in the include and exclude states as the variable is boolean.
    assert(values_only_in_include.size() <= 1 && values_only_in_exclude.size() <= 1);
    if (values_only_in_include.size() == 1) {
      auto form = formulaUtility::makeEqualityFormula(variableName, values_only_in_include.begin()->first, false);
      return form;
    }
    else if (values_only_in_exclude.size() == 1) {
      // Think about this as we need to negative the value
      auto form = formulaUtility::makeEqualityFormula(variableName, values_only_in_exclude.begin()->first, true);
      return form;
    }
    return std::make_shared<BooleanConstant>(true);
  }
  else if (std::holds_alternative<int>(arbitrary_value)) {
    auto value_ranges_overlap = formulaUtility::values_overlap(include_values, exclude_values);
    if (value_ranges_overlap.value_ranges_overlap) {
      // The value ranges overlap. They can only only be distinguished based on the values.
      if (!include_values.empty()) {
        auto equalFormulas = formulaUtility::makeAlternativeFormula(variableName, include_values, false);
        return equalFormulas;
      }
      else if (!exclude_values.empty()) {
        auto notEqualFormulas = formulaUtility::makeAlternativeFormula(variableName, exclude_values, true);
        return notEqualFormulas;
      }
      else {
        throw std::invalid_argument("The variable " + variableName + " has no values in the include or exclude states.");
      }
    }
    else {
      // The value ranges do not overlap - they can be distinguished using a comparison formula such as larger than or
      auto variableFormula = std::make_shared<VariableFormula>(variableName);
      auto valueFormula = std::make_shared<NumberConstant>(value_ranges_overlap.boundary_value);
      if (value_ranges_overlap.is_larger)
        return std::make_shared<LargerEqualsFormula>(variableFormula, valueFormula);
      else
        return std::make_shared<SmallerEqualsFormula>(variableFormula, valueFormula);
    }
  }
  else if (std::holds_alternative<std::string>(arbitrary_value)) {
    if (!include_values.empty()) {
      auto equalFormulas = formulaUtility::makeAlternativeFormula(variableName, include_values, false);
      return equalFormulas;
    }
    else if (!exclude_values.empty()) {
      auto notEqualFormulas = formulaUtility::makeAlternativeFormula(variableName, exclude_values, true);
      return notEqualFormulas;
    }
    else {
      throw std::invalid_argument("The variable " + variableName + " has no values in the include or exclude states.");
    }
  }
  return std::make_shared<BooleanConstant>(true);
}

// Look into how a temporal formula is created.
std::shared_ptr<formula> formulaCreator::groupStatesByFormula(const std::vector<std::shared_ptr<state>> & states, bool temporal)
{
  // The vector of states must not be empty.
  if (states.size() == 0) {
    throw std::invalid_argument("The vector of states is empty.");
  }
  if (states.size() == 1 && temporal) {
    throw std::invalid_argument("The vector of states has only one state, so it cannot be used to create a temporal formula.");
  }
  auto variables = states.front()->getAllVisibleVariables();
  auto variable_val_map = buildVariableValueMap(states);

  // We need to create a formula that groups the states based on the value of the variable.
  auto formulas = std::vector<std::shared_ptr<formula>>();
  // Loop through the variables and create the formula.
  for (auto var : variables) {
    auto name = var->getVisibleName();
    auto values = variable_val_map[name];
    auto arbitrary_value = values.begin()->first;
    if (std::holds_alternative<bool>(arbitrary_value)) {
      if (values.size() > 1) {
        std::cout << "The boolean variable " << name << " is changing in the states, so it cannot be used to group the states."
                  << std::endl;
      }
      else {
        auto localFormula = formulaUtility::makeEqualityFormula(name, values.begin()->first, false);
        formulas.push_back(localFormula);
      }
    }
    else if (std::holds_alternative<int>(arbitrary_value)) {
      int largest_Value = formulaUtility::largestValue(values);
      int smallest_Value = formulaUtility::smallestValue(values);
      // If the difference between the largest and smallest value is equal to the number of states, we can create a range
      if (largest_Value - smallest_Value == (int)(values.size() - 1) && values.size() > 1) {
        auto rangeFormula = formulaUtility::makeRangeFormula(name, smallest_Value, largest_Value);
        formulas.push_back(rangeFormula);
      }
      else {
        auto subformula = formulaUtility::makeAlternativeFormula(name, values, false);
        formulas.push_back(subformula);
      }
    }
    else if (std::holds_alternative<std::string>(arbitrary_value)) {
      auto subformula = formulaUtility::makeAlternativeFormula(name, values, false);
      formulas.push_back(subformula);
    }
    else {
      throw std::invalid_argument("The value of the variable " + name + " is not a boolean, integer, or string.");
    }
  }
  auto result_formula = formulaUtility::combineFormulas(formulas, CombinationOperatorType::AND_Symbol);
  return result_formula;
}

/// @brief Create a formula that distinguishes the states in include_states from the states in exclude_states.
/// @param include_states states to include in the formula
/// @param exclude_states states to exclude from the formula
/// @param temporal if true, the formula covers the entire trace, otherwise it covers states at a specific time
/// @return a formula that distinguishes the states in include_states from the states in exclude_states
std::shared_ptr<formula> formulaCreator::distinguishStates(const std::vector<std::shared_ptr<state>> include_states,
                                                           const std::vector<std::shared_ptr<state>> exclude_states,
                                                           bool temporal)
{
  if (sameStates(include_states, exclude_states)) {
    std::cout << "The states are the same. I am returning a false formula as they cannot be distinguished." << std::endl;
    return std::make_shared<BooleanConstant>(false);
  }

  auto include_variable_val_map = buildVariableValueMap(include_states);
  auto exclude_variable_val_map = buildVariableValueMap(exclude_states);
  // The two maps should have the same keys.
  assert(include_variable_val_map.size() == exclude_variable_val_map.size());
  for (auto var : include_variable_val_map) {
    auto name = var.first;
    if (exclude_variable_val_map.find(name) == exclude_variable_val_map.end()) {
      throw std::invalid_argument("The variable " + name + " is not in the exclude states.");
    }
  }
  // We need to create a formula that groups the states based on the value of the variable.
  std::vector<std::shared_ptr<formula>> subformulas;
  auto not_distinguishing_variables = std::vector<std::string>();

  // Loop through the variables and create the formula.
  for (auto var : include_variable_val_map) {
    auto variableName = var.first;
    auto include_values = var.second;
    auto exclude_values = exclude_variable_val_map[variableName];
    auto variable_formula = createVariableFormula(variableName, include_values, exclude_values);
    auto isTrue = std::dynamic_pointer_cast<BooleanConstant>(variable_formula);
    if (isTrue && isTrue->isTrue()) {
      not_distinguishing_variables.push_back(variableName);
    }
    else {
      subformulas.push_back(variable_formula);
    }
  }
  if (subformulas.empty()) {
    std::cout << "The states cannot be distinguished based on the variables. It is possible that the states are the same. I am "
                 "returning a false formula."
              << std::endl;
    return std::make_shared<BooleanConstant>(false);
  }
  // We need to create a formula that groups the states based on the value of the variable.
  auto combinedFormula = formulaUtility::combineFormulas(subformulas, CombinationOperatorType::AND_Symbol);
  // Make it to an always formula.
  if (temporal) {
    combinedFormula = std::make_shared<GloballyFormula>(combinedFormula);
  }
  return combinedFormula;
}

std::shared_ptr<formula> formulaCreator::createTransitionFormula(const std::shared_ptr<state> current_state,
                                                                 const std::vector<std::shared_ptr<state>> include_states,
                                                                 const std::vector<std::shared_ptr<state>> exclude_states)
{
  auto distinguishingFormula = distinguishStates(include_states, exclude_states, false);
  auto finallyFormula = std::make_shared<FinallyFormula>(distinguishingFormula);
  auto nextFormula = std::make_shared<NextFormula>(finallyFormula);
  auto commonStateFormula = groupStatesByFormula({current_state});
  auto alwaysFormula = std::make_shared<GloballyFormula>(std::make_shared<ImpliesFormula>(commonStateFormula, nextFormula));
  return alwaysFormula;
}

std::pair<std::shared_ptr<trace>, std::shared_ptr<trace>>
formulaCreator::removeCommonPrefixes(const std::shared_ptr<trace> trace1, const std::shared_ptr<trace> trace2)
{
  auto states1 = trace1->getStates();
  auto states2 = trace2->getStates();
  auto transitions1 = trace1->getTransitions();
  auto transitions2 = trace2->getTransitions();
  int minimum_size = std::min(states1.size(), states2.size());
  for (int i = 0; i < minimum_size - 1; i++) {
    auto state1 = states1[i];
    auto state2 = states2[i];
    auto next_state1 = states1[i + 1];
    auto next_state2 = states2[i + 1];
    auto next_same = next_state1->isSame(next_state2.get());
    if (state1->isSame(state2.get()) && next_same) {
      trace1->removeStateAt(0);
      trace2->removeStateAt(0);
      trace1->removeTransitionAt(0);
      trace2->removeTransitionAt(0);
    }
    else {
      break;
    }
  }
  auto first_state1 = trace1->getStates().front();
  auto first_state2 = trace2->getStates().front();
  assert(first_state1->isSame(first_state2.get()));
  assert(first_state1->isSame(first_state2.get()));
  auto next_state1 = trace1->getStates().at(1);
  auto next_state2 = trace2->getStates().at(1);
  assert(!next_state1->isSame(next_state2.get()));
  next_state1->printDelta(next_state2.get());
  return std::pair<std::shared_ptr<trace>, std::shared_ptr<trace>>(trace1, trace2);
}