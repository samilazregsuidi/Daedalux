#pragma once

#include "formula.hpp"
#include "formulaUtility.hpp"
#include "state.hpp"
#include "trace.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

class formulaCreator {
public:
  // static std::string distinguishStates(const std::shared_ptr<state> & state1, const std::shared_ptr<state> & state2);
  static std::shared_ptr<formula> groupStatesByFormula(const std::vector<std::shared_ptr<state>> & states,
                                                       bool temporal = false);
  static std::unordered_map<std::string, ValueStateMap>
  buildVariableValueMap(const std::vector<std::shared_ptr<state>> & states);

  static std::vector<ValueType> getVariableValues(const std::vector<std::shared_ptr<state>> & states,
                                                  const std::string & variableName);

  static std::shared_ptr<formula> distinguishTraces(const std::shared_ptr<trace> & include_trace,
                                                    const std::shared_ptr<trace> & exclude_trace);

  static std::shared_ptr<formula> distinguishStates(const std::vector<std::shared_ptr<state>> & include_states,
                                                    const std::vector<std::shared_ptr<state>> & exclude_states,
                                                    bool temporal = false);

  static std::shared_ptr<formula> createTransitionFormula(const std::shared_ptr<state> current_state,
                                                          const std::vector<std::shared_ptr<state>> & include_states,
                                                          const std::vector<std::shared_ptr<state>> & exclude_states,
                                                          bool negate = false);

  static std::shared_ptr<formula>
  createVariableFormula(std::string variableName, ValueStateMap include_values,
                        ValueStateMap exclude_values);

  static std::pair<std::shared_ptr<trace>, std::shared_ptr<trace>> removeCommonPrefixes(const std::shared_ptr<trace> trace1,
                                                                                        const std::shared_ptr<trace> trace2);
};