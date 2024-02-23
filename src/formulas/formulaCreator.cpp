#include "formulaCreator.hpp"
#include "ltl.hpp"
#include "primitiveVariable.hpp"
#include "state.hpp"
#include <algorithm> // Include the necessary header file
#include <iostream>
#include <iterator> // Include the necessary header file
#include <numeric>
#include <sstream>

std::string formulaCreator::distinguishTraces(const std::shared_ptr<trace> & include_trace,
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
  distinguishStates(successor1_vec, successor2_vec);
  return "";
}

std::unordered_map<std::string, std::map<int, std::vector<std::shared_ptr<state>>>>
formulaCreator::buildVariableValueMap(const std::vector<std::shared_ptr<state>> & states)
{
  // The vector of states must not be empty.
  assert(states.size() > 0);
  auto variables = states.front()->getAllVariables();
  auto variable_val_map = std::unordered_map<std::string, std::map<int, std::vector<std::shared_ptr<state>>>>();
  for (auto var : variables) {
    std::string name = var->getLocalName();
    std::string fullName = var->getFullName();
    auto value_map = std::map<int, std::vector<std::shared_ptr<state>>>();
    for (auto stateVar : states) {
      auto value = getValueOfVariable(stateVar, name);
      value_map[value].push_back(stateVar);
    }
    variable_val_map[name] = value_map;
  }
  return variable_val_map;
}

void formulaCreator::groupStates(const std::vector<std::shared_ptr<state>> & states)
{
  // The vector of states must not be empty.
  assert(states.size() > 0);
  auto variables = states.front()->getAllVariables();
  auto variable_val_map = buildVariableValueMap(states);

  // We need to create a formula that groups the states based on the value of the variable.
  std::string formula = "";
  // Loop through the variables and create the formula.
  for (auto var : variables) {
    auto name = var->getLocalName();
    auto values = variable_val_map[name];
    if (values.size() <= 1) {
      // The variable has the same value in all states.
      variable_val_map.erase(name);
      // We do not need to consider this variable.
      continue;
    }
    auto largestElement =
        std::max_element(values.begin(), values.end(), [](const auto & a, const auto & b) { return a.first < b.first; });

    auto smallestElement =
        std::min_element(values.begin(), values.end(), [](const auto & a, const auto & b) { return a.first < b.first; });

    const int largestValue = largestElement->first;
    const int smallestValue = smallestElement->first;

    // The variable has different values in the states.
    std::cout << "The variable " << name << " has different values in the states." << std::endl;
    std::cout << "The biggest value is " << largestValue << " and the smallest value is " << smallestValue << std::endl;

    formula += "(";
    for (auto value : values) {
      formula += name + " = " + std::to_string(value.first) + " || ";
    }
  }
  std::cout << "The formula is " << formula << std::endl;
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
  auto largestElement_range1 =
      std::max_element(values1.begin(), values1.end(), [](const auto & a, const auto & b) { return a.first < b.first; });
  auto smallestElement_range1 =
      std::min_element(values1.begin(), values1.end(), [](const auto & a, const auto & b) { return a.first < b.first; });
  auto largestElement_range2 =
      std::max_element(values2.begin(), values2.end(), [](const auto & a, const auto & b) { return a.first < b.first; });
  auto smallestElement_range2 =
      std::min_element(values2.begin(), values2.end(), [](const auto & a, const auto & b) { return a.first < b.first; });
  if (largestElement_range1->first >= smallestElement_range2->first &&
      smallestElement_range2->first <= smallestElement_range1->first) {
    value_ranges_overlap = true;
  }
  if (largestElement_range2->first >= smallestElement_range1->first &&
      largestElement_range2->first <= largestElement_range1->first) {
    value_ranges_overlap = true;
  }

  overlapResult result;
  result.value_ranges_overlap = value_ranges_overlap;
  if (!value_ranges_overlap) {
    if (largestElement_range1->first > largestElement_range2->first) {
      result.boundary_value = largestElement_range2->first;
      result.is_larger = false;
    }
    else {
      result.boundary_value = smallestElement_range2->first;
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

void formulaCreator::distinguishStates(const std::vector<std::shared_ptr<state>> include_states,
                                       const std::vector<std::shared_ptr<state>> exclude_states)
{
  // We need to create a formula that distinguishes the states in include_states from the states in exclude_states.
  // We need to group the states based on the value of the variables.
  // We need to create a formula that distinguishes the states in include_states from the states in exclude_states.
  // We need to group the states based on the value of the variables.
  auto include_variable_val_map = buildVariableValueMap(include_states);
  auto exclude_variable_val_map = buildVariableValueMap(exclude_states);
  // The two maps should have the same keys.
  assert(include_variable_val_map.size() == exclude_variable_val_map.size());
  for (auto var : include_variable_val_map) {
    auto name = var.first;
    assert(exclude_variable_val_map.find(name) != exclude_variable_val_map.end());
  }
  // We need to create a formula that groups the states based on the value of the variable.
  std::string formula = "";
  std::vector<std::string> subformulas;
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

    std::string subFormula = "(";

    auto value_ranges_overlap = values_overlap(include_values, exclude_values);
    if (value_ranges_overlap.value_ranges_overlap) {
      std::cout << "The variable " << variableName
                << " has overlapping values, so we need to distinguish them based on the values." << std::endl;
      for (auto value : values_only_in_include) {
        subFormula += variableName + " = " + std::to_string(value.first) + " || ";
      }
      for (auto value : values_only_in_exclude) {
        subFormula += variableName + " != " + std::to_string(value.first) + " || ";
      }
    }
    else {
      std::cout << "The two value ranges for the variable " << variableName
                << " do not overlap. Meaning a boundary "
                   "condition can be used to distinguish the states."
                << std::endl;
      if (value_ranges_overlap.is_larger) {
        subFormula += variableName + " > " + std::to_string(value_ranges_overlap.boundary_value);
      }
      else {
        subFormula += variableName + " < " + std::to_string(value_ranges_overlap.boundary_value);
      }
    }
    subFormula += ")";
    subformulas.push_back(subFormula);
  }

  if (not_distinguishing_variables.size() == include_variable_val_map.size()) {
    std::cout << "The states cannot be distinguished based on the variables." << std::endl;
  }
  else {
    std::cout << "The states can be distinguished based on the variables." << std::endl;
    formula = std::accumulate(std::begin(subformulas), std::end(subformulas), std::string(),
                              [](std::string & ss, std::string & s) { return ss.empty() ? s : ss + " && " + s; });
  }

  std::cout << "The formula is " << formula << std::endl;
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
    casted_variable = dynamic_cast<primitiveVariable *>(variable);
    assert(casted_variable);
    value = casted_variable->getValue();
    break;
  case variable::V_UTYPE:
    casted_variable = dynamic_cast<primitiveVariable *>(variable);
    assert(casted_variable);
    // value = casted_variable->getValue();
    break;
  default:
    break;
  }
  return value;
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

std::string formulaCreator::formulaStringToNeverClaim(const std::string & formula)
{
  auto never_claim = transformLTLStringToNeverClaim(formula);
  return never_claim;
}
