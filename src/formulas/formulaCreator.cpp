#include "formulaCreator.hpp"
#include "formula.hpp"
#include "ltl.hpp"
#include "primitiveVariable.hpp"
#include "state.hpp"
#include <algorithm>
#include <iostream>
#include <iterator> // for std::begin, std::end
#include <numeric>  // for std::accumulate
#include <sstream>


std::unordered_map<std::string, std::map<int, std::vector<std::shared_ptr<state>>>>
formulaCreator::buildVariableValueMap(const std::vector<std::shared_ptr<state>> & states)
{
  // The vector of states must not be empty.
  assert(states.size() > 0);
  // Get all variables that are visible in the states.
  auto variables = states.front()->getAllVisibleVariables();
  auto variable_val_map = std::unordered_map<std::string, std::map<int, std::vector<std::shared_ptr<state>>>>();
  for (auto var : variables) {
    std::string name = var->getLocalName();
    auto value_map = std::map<int, std::vector<std::shared_ptr<state>>>();
    for (auto stateVar : states) {
      auto value = getValueOfVariable(stateVar, name);
      value_map[value].push_back(stateVar);
    }
    variable_val_map[name] = value_map;
  }
  return variable_val_map;
}

std::shared_ptr<formula> formulaCreator::distinguishTraces(const std::shared_ptr<trace> & include_trace,
                                                           const std::shared_ptr<trace> & exclude_trace)
{
  std::shared_ptr<state> previous_state_current = nullptr;
  std::shared_ptr<state> previous_state_other = nullptr;
  auto [include_trace_short, exclude_trace_short] = removeCommonPrefixes(include_trace, exclude_trace);
  auto state1 = include_trace_short->getStates().front();
  auto state2 = exclude_trace_short->getStates().front();
  assert(state1->isSame(state2.get()));
  auto successors1 = state1->Post();
  auto successors2 = state2->Post();
  auto successor1_vec = std::vector<std::shared_ptr<state>>(successors1.begin(), successors1.end());
  auto successor2_vec = std::vector<std::shared_ptr<state>>(successors2.begin(), successors2.end());
  auto successor1 = successor1_vec.front();
  auto successor2 = successor2_vec.front();
  successor1->printDelta(successor2.get());
  auto distinguishingFormula = distinguishStates(successor1_vec, successor2_vec, false);
  auto finallyFormula = std::make_shared<FinallyFormula>(distinguishingFormula);
  return finallyFormula;
}


std::shared_ptr<formula> formulaCreator::makeRangeFormula(std::string name, int smallestValue, int largestValue)
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

std::shared_ptr<formula> formulaCreator::makeEqualFormula(std::string name, int value)
{
  auto formulaVar = std::make_shared<VariableFormula>(name);
  auto valueFormula = std::make_shared<NumberConstant>(value);
  auto local_formula = std::make_shared<EqualsFormula>(formulaVar, valueFormula);
  return local_formula;
}

std::shared_ptr<formula> formulaCreator::makeBooleanEqualFormula(std::string name, bool value)
{
  auto formulaVar = std::make_shared<VariableFormula>(name);
  auto valueFormula = std::make_shared<BooleanConstant>(value);
  auto local_formula = std::make_shared<EqualsFormula>(formulaVar, valueFormula);
  return local_formula;
}

int largestValue(const std::map<int, std::vector<std::shared_ptr<state>>> & values)
{
  auto largestElement =
      std::max_element(values.begin(), values.end(), [](const auto & a, const auto & b) { return a.first < b.first; });
  return largestElement->first;
}

int smallestValue(const std::map<int, std::vector<std::shared_ptr<state>>> & values)
{
  auto smallestElement =
      std::min_element(values.begin(), values.end(), [](const auto & a, const auto & b) { return a.first < b.first; });
  return smallestElement->first;
}

bool isConstant(const std::map<int, std::vector<std::shared_ptr<state>>> & values) { return values.size() == 1; }

std::shared_ptr<formula> formulaCreator::groupStatesByFormula(const std::vector<std::shared_ptr<state>> & states)
{
  // The vector of states must not be empty.
  assert(states.size() > 0);
  auto variables = states.front()->getAllVisibleVariables();
  auto variable_val_map = buildVariableValueMap(states);

  // We need to create a formula that groups the states based on the value of the variable.
  auto formulas = std::vector<std::shared_ptr<formula>>();
  // Loop through the variables and create the formula.
  for (auto var : variables) {
    auto name = var->getLocalName();
    auto values = variable_val_map[name];
    auto isBoolean = isBooleanVariable(states.front(), name);
    if (isConstant(values)) {
      // The variable has the same value in all states. It is an invariant/global formula.
      auto value = values.begin()->first;
      auto localFormula = isBoolean ? makeBooleanEqualFormula(name, value) : makeEqualFormula(name, value);
      auto globalFormula = std::make_shared<GloballyFormula>(localFormula);
      formulas.push_back(globalFormula);
    }
    else {
      if (isBoolean) {
        // We can only have one value in the include and exclude states as the variable is boolean.
        if (values.size() > 1) {
          std::cout << "The boolean variable " << name
                    << " is changing in the states, so it cannot be used to group the states." << std::endl;
        }
        else {
          auto value = values.begin()->first;
          auto localFormula = makeBooleanEqualFormula(name, value);
          formulas.push_back(localFormula);
        }
      }
      else {
        // The variable is not boolean, but it is a number. 
        const int largest_Value = largestValue(values);
        const int smallest_Value = smallestValue(values);
        // If the difference between the largest and smallest value is equal to the number of states, we can create a range
        if (largest_Value - smallest_Value == (int)(values.size() - 1)) {
          auto rangeFormula = makeRangeFormula(name, smallest_Value, largest_Value);
          formulas.push_back(rangeFormula);
        }
        else {
          auto subformulas = std::vector<std::shared_ptr<formula>>();
          for (auto value : values) {
            auto localFormula = makeEqualFormula(name, value.first);
            subformulas.push_back(localFormula);
          }
          auto local_formula = groupFormulas(subformulas, "||");
          auto parent_formula = std::make_shared<ParenthesisFormula>(local_formula);
          formulas.push_back(parent_formula);
        }
      }
    }
  }
  auto result_formula = groupFormulas(formulas, "&&");
  return result_formula;
}

struct overlapResult {
  // Specifies if the value ranges overlap between the include and exclude states.
  bool value_ranges_overlap;
  // The smallest/largest value in the the exclude states, if the value ranges do not overlap.
  int boundary_value;
  // Specifies if the boundary value is larger or smaller than the value in the exclude states.
  bool is_larger;
};

overlapResult values_overlap(const std::map<int, std::vector<std::shared_ptr<state>>> & values1,
                             const std::map<int, std::vector<std::shared_ptr<state>>> & values2)
{
  bool value_ranges_overlap = false;
  auto largestValue_range1 = largestValue(values1);
  auto smallestValue_range1 = smallestValue(values1);
  auto largestValue_range2 = largestValue(values2);
  auto smallestValue_range2 = smallestValue(values2);
  if (largestValue_range1 >= smallestValue_range2 && smallestValue_range2 <= smallestValue_range1) {
    value_ranges_overlap = true;
  }
  if (largestValue_range2 >= smallestValue_range1 && largestValue_range2 <= largestValue_range1) {
    value_ranges_overlap = true;
  }
  overlapResult result;
  result.value_ranges_overlap = value_ranges_overlap;
  if (!value_ranges_overlap) {
    if (largestValue_range1 > largestValue_range2) {
      result.boundary_value = largestValue_range2;
      result.is_larger = false;
    }
    else {
      result.boundary_value = smallestValue_range2;
      result.is_larger = true;
    }
  }
  return result;
}

std::map<int, std::vector<std::shared_ptr<state>>>
remove_duplicated_values(const std::map<int, std::vector<std::shared_ptr<state>>> & values1,
                         const std::map<int, std::vector<std::shared_ptr<state>>> & values2)
{
  std::map<int, std::vector<std::shared_ptr<state>>> new_values;
  for (auto value : values1) {
    if (values2.find(value.first) == values2.end()) {
      new_values[value.first] = value.second;
    }
  }
  return new_values;
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
  auto include_variable_val_map = buildVariableValueMap(include_states);
  auto exclude_variable_val_map = buildVariableValueMap(exclude_states);
  // The two maps should have the same keys.
  assert(include_variable_val_map.size() == exclude_variable_val_map.size());
  for (auto var : include_variable_val_map) {
    auto name = var.first;
    assert(exclude_variable_val_map.find(name) != exclude_variable_val_map.end());
  }
  // We need to create a formula that groups the states based on the value of the variable.
  std::vector<std::shared_ptr<formula>> subformulas;
  auto not_distinguishing_variables = std::vector<std::string>();

  // Loop through the variables and create the formula.
  for (auto var : include_variable_val_map) {
    auto variableName = var.first;
    auto include_values = var.second;
    auto exclude_values = exclude_variable_val_map[variableName];
    auto values_only_in_include = remove_duplicated_values(include_values, exclude_values);
    auto values_only_in_exclude = remove_duplicated_values(exclude_values, include_values);

    if (values_only_in_include.size() == 0 && values_only_in_exclude.size() == 0) {
      // The variable has the same value in all states.
      not_distinguishing_variables.push_back(variableName);
      // We do not need to consider this variable.
      continue;
    }

    auto variableFormula = std::make_shared<VariableFormula>(variableName);

    if (isBooleanVariable(include_states.front(), variableName)) {
      // We can only have one value in the include and exclude states as the variable is boolean.
      assert(values_only_in_include.size() <= 1 && values_only_in_exclude.size() <= 1);
      auto addFormula = [&](bool include, const auto & values) {
        if (!values.empty()) {
          auto value = values.begin()->first;
          bool isTrue = include ? value == 1 : value == 0;
          auto form = makeBooleanEqualFormula(variableName, isTrue);
          subformulas.push_back(form);
        }
      };
      if (values_only_in_include.size() == 1) {
        addFormula(true, values_only_in_include);
      }
      else if (values_only_in_exclude.size() == 1) {
        addFormula(false, values_only_in_exclude);
      }
      continue;
    }

    auto value_ranges_overlap = values_overlap(include_values, exclude_values);
    if (value_ranges_overlap.value_ranges_overlap) {
      // The value ranges overlap. They should be distinguished based on the values.
      auto createFormulas = [&](std::map<int, std::vector<std::shared_ptr<state>>> values, bool include) {
        for (auto value : values) {
          if (include) {
            auto localFormula = makeEqualFormula(variableName, value.first);
            subformulas.push_back(localFormula);
          }
          else {
            auto valueFormula = std::make_shared<NumberConstant>(value.first);
            auto localFormula = std::make_shared<NotEqualsFormula>(variableFormula, valueFormula);
            subformulas.push_back(localFormula);
          }
        }
      };
      createFormulas(values_only_in_include, true);
      createFormulas(values_only_in_exclude, false);
    }
    else {
      // The value ranges do not overlap - they can be distinguished using a comparison formula such as larger than or
      auto valueFormula = std::make_shared<NumberConstant>(value_ranges_overlap.boundary_value);
      auto localFormula = value_ranges_overlap.is_larger ? std::make_shared<LargerEqualsFormula>(variableFormula, valueFormula)
                                                         : std::make_shared<LargerEqualsFormula>(valueFormula, variableFormula);
      subformulas.push_back(localFormula);
    }
  }

  if (not_distinguishing_variables.size() == include_variable_val_map.size() || subformulas.empty()) {
    std::cout << "The states cannot be distinguished based on the variables. It is possible that the states are the same. I am "
                 "returning a false formula."
              << std::endl;
    return std::make_shared<BooleanConstant>(false);
  }
  // We need to create a formula that groups the states based on the value of the variable.
  auto combinedFormula = groupFormulas(subformulas, "&&");
  // Make it to an always formula.
  if (temporal) {
    auto globalFormula = std::make_shared<GloballyFormula>(combinedFormula);
    return globalFormula;
  }
  return combinedFormula;
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
  auto next_state1 = trace1->getStates().at(1);
  auto next_state2 = trace2->getStates().at(1);
  assert(!next_state1->isSame(next_state2.get()));
  return std::pair<std::shared_ptr<trace>, std::shared_ptr<trace>>(trace1, trace2);
}

bool formulaCreator::isBooleanVariable(const std::shared_ptr<state> & state, const std::string & name)
{
  auto variable = state->getVariable(name);
  auto type = variable->getType();
  return type == variable::V_BOOL || type == variable::V_BIT;
}

int formulaCreator::getValueOfVariable(const std::shared_ptr<state> & state, const std::string & name)
{
  auto variable = state->getVariable(name);
  primitiveVariable * casted_variable = nullptr;
  auto value = 0;
  switch (variable->getType()) {
  case variable::V_BOOL:
  case variable::V_BIT:
  case variable::V_BYTE:
  case variable::V_PID:
  case variable::V_CID:
  case variable::V_SHORT:
  case variable::V_INT:
    casted_variable = dynamic_cast<primitiveVariable *>(variable);
    assert(casted_variable);
    value = casted_variable->getValue();
    break;
  case variable::V_MTYPE:
  case variable::V_CMTYPE:
    // Need to implement
    casted_variable = dynamic_cast<primitiveVariable *>(variable);
    assert(casted_variable);
    value = casted_variable->getValue();
    break;
  case variable::V_UTYPE:
    // Need to implement
    // casted_variable = dynamic_cast<uTypeVar *>(variable);
    // assert(casted_variable);
    // value = casted_variable->getValue();
    break;
  default:
    break;
  }
  return value;
}

std::shared_ptr<formula> formulaCreator::groupFormulas(const std::vector<std::shared_ptr<formula>> & formulas,
                                                       const std::string & operatorSymbol)
{
  if (operatorSymbol != "&&" && operatorSymbol != "||") {
    throw std::invalid_argument("The operator must be '&&' or '||'");
  }
  if (formulas.empty()) {
    std::cout << "The vector of formulas is empty. I am returning a true formula." << std::endl;
    return std::make_shared<BooleanConstant>(true);
  }
  if (formulas.size() == 1) {
    return formulas.front();
  }
 if (operatorSymbol == "&&") {
    auto formula = std::make_shared<AndFormula>(formulas.front(), formulas.at(1));
    for (size_t i = 2; i < formulas.size(); i++) {
      formula = std::make_shared<AndFormula>(formula, formulas.at(i));
    }
    return formula;
  }
  else {
    auto formula = std::make_shared<OrFormula>(formulas.front(), formulas.at(1));
    for (size_t i = 2; i < formulas.size(); i++) {
      formula = std::make_shared<OrFormula>(formula, formulas.at(i));
    }
    return formula;
  }
}