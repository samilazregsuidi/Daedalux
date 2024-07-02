#include "formulaCreator.hpp"
#include "formula.hpp"
#include "ltl.hpp"
#include "state.hpp"
#include "scalarVarInt.hpp"
#include <algorithm> // Include the necessary header file
#include <iostream>
#include <iterator> // for std::begin, std::end
#include <numeric> // for std::accumulate
#include <sstream>
#include <utils/stateComparer.hpp>
#include <variable.hpp>

enum VariableValueType { BOOL, INT, STRING };

VariableValueType getVariableType(const ValueStateMap & values)
{
  auto arbitrary_value = values.begin()->first;
  if (std::holds_alternative<bool>(arbitrary_value)) {
    return VariableValueType::BOOL;
  }
  else if (std::holds_alternative<int>(arbitrary_value)) {
    return VariableValueType::INT;
  }
  else if (std::holds_alternative<std::string>(arbitrary_value)) {
    return VariableValueType::STRING;
  }
  else {
    throw std::invalid_argument("The value of the variable is not a boolean, integer, or string.");
  }
}

bool isConstant(const ValueStateMap & values) { return values.size() == 1; }

std::unordered_map<std::string, ValueStateMap>
formulaCreator::buildVariableValueMap(const std::vector<std::shared_ptr<state>> & states)
{
  // The vector of states must not be empty.
  if (states.empty()) {
    return std::unordered_map<std::string, ValueStateMap>();
  }
  // Get all variables that are visible in the states.
  auto variables = states.front()->getAllVisibleVariables();
  std::unordered_map<std::string, ValueStateMap> variable_val_map;
  for (auto var : variables) {
    std::string name = var->getVisibleName();
    ValueStateMap value_map;
    for (auto stateVar : states) {
      auto value = formulaUtility::getValueOfVariable(stateVar.get(), name);
      if (value_map.find(value) == value_map.end()) {
        value_map[value] = std::vector<std::shared_ptr<state>>();
      }
      value_map[value].push_back(stateVar);
    }
    variable_val_map[name] = value_map;
  }
  return variable_val_map;
}

std::vector<ValueType> formulaCreator::getVariableValues(const std::vector<std::shared_ptr<state>> & states,
                                                         const std::string & variableName)
{
  std::vector<ValueType> values;
  for (auto state : states) {
    auto value = formulaUtility::getValueOfVariable(state.get(), variableName);
    if (std::find(values.begin(), values.end(), value) == values.end()) {
      values.push_back(value);
    }
  }
  return values;
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
  assert(state1->isSame(state2.get(), false));
  auto successors1 = state1->SafePost();
  auto successors2 = state2->SafePost();
  auto successor1_vec = std::vector<std::shared_ptr<state>>(successors1.begin(), successors1.end());
  auto successor2_vec = std::vector<std::shared_ptr<state>>(successors2.begin(), successors2.end());
  bool considerInternalVariables = false;
  if (StateComparer::sameStates(successor1_vec, successor2_vec, considerInternalVariables)) {
    std::cout << "The successors are the same. I am returning a false formula as they cannot be distinguished." << std::endl;
    return std::make_shared<BooleanConstant>(false);
  }
  auto transitionFormula = createTransitionFormula(state1, successor1_vec, successor2_vec);
  return transitionFormula;
}

ValueStateMap
remove_duplicated_values(const ValueStateMap & values1,
                         const ValueStateMap & values2)
{
  ValueStateMap new_values;
  for (auto value : values1) {
    if (values2.find(value.first) == values2.end()) {
      new_values[value.first] = value.second;
    }
  }
  return new_values;
}

/// @brief This function creates a formula that distinguishes a variable based on the values in the include and exclude states.
/// @param variableName is the name of the variable
/// @param include_values is a map of values which we want to include in the formula and the states that have the value
/// @param exclude_values is a map of values which we want to exclude from the formula and the states that have the value
/// @return a formula that distinguishes the variable based on the values in the include and exclude states
std::shared_ptr<formula>
formulaCreator::createVariableFormula(std::string variableName,
                                      ValueStateMap include_values,
                                      ValueStateMap exclude_values)
{
  // Remove the common values from the include and exclude values.
  auto values_only_in_include = remove_duplicated_values(include_values, exclude_values);
  auto values_only_in_exclude = remove_duplicated_values(exclude_values, include_values);
  // If the variable has the same value in all states, it cannot be used to distinguish the states.
  if (values_only_in_include.empty() && values_only_in_exclude.empty()) {
    return std::make_shared<BooleanConstant>(true);
  }
  // Get an arbitrary value from the include or exclude values used to find the type of the variable.
  auto arbitrary_value =
      values_only_in_include.empty() ? values_only_in_exclude.begin()->first : values_only_in_include.begin()->first;

  auto variableType = getVariableType(include_values);

  if (variableType == VariableValueType::BOOL) {
    if (values_only_in_include.size() > 1 || values_only_in_exclude.size() > 1) {
      // The variable is boolean and it has more than one value so it cannot be used to distinguish anything
      return std::make_shared<BooleanConstant>(true);
    }
    if (values_only_in_include.size() == 1) {
      return formulaUtility::makeEqualityFormula(variableName, values_only_in_include.begin()->first, false);
    }
    else if (values_only_in_exclude.size() == 1) {
      return formulaUtility::makeEqualityFormula(variableName, values_only_in_exclude.begin()->first, true);
    }
  }
  else if (variableType == VariableValueType::INT) {
    auto value_ranges_overlap = formulaUtility::values_overlap(include_values, exclude_values);
    if (value_ranges_overlap.value_ranges_overlap) {
      // The value ranges overlap. They can only only be distinguished based on the values.
      return formulaUtility::createEqualityFormulas(variableName, include_values, exclude_values);
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
  else if (variableType == VariableValueType::STRING) {
    return formulaUtility::createEqualityFormulas(variableName, include_values, exclude_values);
  }
  return std::make_shared<BooleanConstant>(true);
}

// Look into how a temporal formula is created.
std::shared_ptr<formula> formulaCreator::groupStatesByFormula(const std::vector<std::shared_ptr<state>> & states, bool temporal)
{
  if (states.size() == 0) {
    throw std::invalid_argument("The vector of states is empty.");
  }
  if (states.size() == 1 && temporal) {
    throw std::invalid_argument("The vector of states has only one state, so it cannot be used to create a temporal formula.");
  }
  auto variables = states.front()->getAllVisibleVariables();
  auto variable_val_map = buildVariableValueMap(states);
  auto empty_values = ValueStateMap();

  // We need to create a formula that groups the states based on the value of the variable.
  auto formulas = std::vector<std::shared_ptr<formula>>();
  // Loop through the variables and create the formula.
  for (auto var : variables) {
    auto name = var->getVisibleName();
    auto values = variable_val_map[name];
    auto variable_formula = createVariableFormula(name, values, empty_values);
    formulas.push_back(variable_formula);
  }
  auto result_formula = formulaUtility::combineFormulas(formulas, CombinationOperatorType::AND_Symbol);
  return result_formula;
}

/// @brief Create a formula that distinguishes the states in include_states from the states in exclude_states.
/// @param include_states states to include in the formula
/// @param exclude_states states to exclude from the formula
/// @param temporal if true, the formula covers the entire trace, otherwise it covers states at a specific time
/// @return a formula that distinguishes the states in include_states from the states in exclude_states
std::shared_ptr<formula> formulaCreator::distinguishStates(const std::vector<std::shared_ptr<state>> & include_states,
                                                           const std::vector<std::shared_ptr<state>> & exclude_states,
                                                           bool temporal)
{
  bool considerInternalVariables = false;
  if (StateComparer::sameStates(include_states, exclude_states, considerInternalVariables)) {
    std::cout << "The states are the same. I am returning a false formula as they cannot be distinguished." << std::endl;
    return std::make_shared<BooleanConstant>(false);
  }

  auto include_variable_val_map = buildVariableValueMap(include_states);
  auto exclude_variable_val_map = buildVariableValueMap(exclude_states);
  assert(include_variable_val_map.size() == exclude_variable_val_map.size());
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
    if (StateComparer::sameStates(include_states, exclude_states, considerInternalVariables)) {
      std::cout << "The states are the same. I am returning a false formula as they cannot be distinguished." << std::endl;
      return std::make_shared<BooleanConstant>(false);
    }
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

// /// @brief Create a formula that distinguishes the states in include_states from the states in exclude_states.
// /// @param include_states states to include in the formula
// /// @param exclude_states states to exclude from the formula
// /// @param temporal if true, the formula covers the entire trace, otherwise it covers states at a specific time
// /// @return a formula that distinguishes the states in include_states from the states in exclude_states
// std::shared_ptr<formula> formulaCreator::distinguishStates(const std::vector<state *> include_states,
//                                                            const std::vector<state *> exclude_states, bool temporal)
// {
//   auto include_variable_val_map = buildVariableValueMap(include_states);
//   auto exclude_variable_val_map = buildVariableValueMap(exclude_states);
//   // The two maps should have the same keys.
//   assert(include_variable_val_map.size() == exclude_variable_val_map.size());
//   for (auto var : include_variable_val_map) {
//     auto name = var.first;
//     if (exclude_variable_val_map.find(name) == exclude_variable_val_map.end()) {
//       throw std::invalid_argument("The variable " + name + " is not in the exclude states.");
//     }
//   }
//   // We need to create a formula that groups the states based on the value of the variable.
//   std::vector<std::shared_ptr<formula>> subformulas;
//   auto not_distinguishing_variables = std::vector<std::string>();

//   // Loop through the variables and create the formula.
//   for (auto var : include_variable_val_map) {
//     auto variableName = var.first;
//     auto include_values = var.second;
//     auto exclude_values = exclude_variable_val_map[variableName];
//     auto variable_formula = createVariableFormula(variableName, include_values, exclude_values);
//     auto isTrue = std::dynamic_pointer_cast<BooleanConstant>(variable_formula);
//     if (isTrue && isTrue->isTrue()) {
//       not_distinguishing_variables.push_back(variableName);
//     }
//     else {
//       subformulas.push_back(variable_formula);
//     }
//   }
//   if (subformulas.empty()) {
//     std::cout << "The states cannot be distinguished based on the variables. It is possible that the states are the same. I
//     am "
//                  "returning a false formula."
//               << std::endl;
//     return std::make_shared<BooleanConstant>(false);
//   }
//   // We need to create a formula that groups the states based on the value of the variable.
//   auto combinedFormula = formulaUtility::combineFormulas(subformulas, CombinationOperatorType::AND_Symbol);
//   // Make it to an always formula.
//   if (temporal) {
//     combinedFormula = std::make_shared<GloballyFormula>(combinedFormula);
//   }
//   return combinedFormula;
// }

std::shared_ptr<formula> formulaCreator::createTransitionFormula(const std::shared_ptr<state> current_state,
                                                                 const std::vector<std::shared_ptr<state>> & include_states,
                                                                 const std::vector<std::shared_ptr<state>> & exclude_states,
                                                                 bool negate)
{
  std::shared_ptr<formula> distinguishingFormula;
  if (!exclude_states.empty()) {
    distinguishingFormula = distinguishStates(include_states, exclude_states, false);
  }
  else {
    distinguishingFormula = groupStatesByFormula(include_states);
  }
  auto finallyFormula = std::make_shared<FinallyFormula>(distinguishingFormula);
  auto nextFormula = std::make_shared<NextFormula>(finallyFormula);
  auto commonStateFormula = groupStatesByFormula({current_state});
  auto impliesFormula = std::make_shared<ImpliesFormula>(commonStateFormula, nextFormula);
  auto parentFormula = std::make_shared<ParenthesisFormula>(impliesFormula);
  std::shared_ptr<formula> negatedFormula = negate
                                                ? std::static_pointer_cast<formula>(std::make_shared<NotFormula>(parentFormula))
                                                : std::static_pointer_cast<formula>(parentFormula);
  auto alwaysFormula = std::make_shared<GloballyFormula>(negatedFormula);
  return alwaysFormula;
}

std::pair<std::shared_ptr<trace>, std::shared_ptr<trace>>
formulaCreator::removeCommonPrefixes(const std::shared_ptr<trace> trace1, const std::shared_ptr<trace> trace2)
{
  bool considerInternalVariables = false;
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
    auto next_same = next_state1->isSame(next_state2.get(), considerInternalVariables);
    if (state1->isSame(state2.get(), considerInternalVariables) && next_same) {
      trace1->removeStateAt(0);
      trace2->removeStateAt(0);
    }
    else {
      break;
    }
  }
  auto first_state1 = trace1->getStates().front();
  auto first_state2 = trace2->getStates().front();
  assert(first_state1->isSame(first_state2.get(), considerInternalVariables));
  assert(first_state1->isSame(first_state2.get(), considerInternalVariables));
  auto next_state1 = trace1->getStates().at(1);
  auto next_state2 = trace2->getStates().at(1);
  assert(!next_state1->isSame(next_state2.get(), considerInternalVariables));
  next_state1->printDelta(next_state2.get(), considerInternalVariables);
  return std::pair<std::shared_ptr<trace>, std::shared_ptr<trace>>(trace1, trace2);
}