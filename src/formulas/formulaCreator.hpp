#pragma once

#include "formula.hpp"
#include "state.hpp"
#include "trace.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class formulaCreator {
public:
  static std::string distinguishStates(const std::shared_ptr<state> & state1, const std::shared_ptr<state> & state2);
  static std::shared_ptr<formula> groupStatesByFormula(const std::vector<std::shared_ptr<state>> & states);
  static std::unordered_map<std::string, std::map<int, std::vector<std::shared_ptr<state>>>>
  buildVariableValueMap(const std::vector<std::shared_ptr<state>> & states);
  static std::shared_ptr<formula> distinguishTraces(const std::shared_ptr<trace> & include_trace, const std::shared_ptr<trace> & exclude_trace);
  static std::shared_ptr<formula> distinguishStates(
                                    const std::vector<std::shared_ptr<state>> include_states,
                                    const std::vector<std::shared_ptr<state>> exclude_states,
                                    bool temporal = false);

  static std::pair<std::shared_ptr<trace>, std::shared_ptr<trace>> removeCommonPrefixes(const std::shared_ptr<trace> trace1,
                                                                                        const std::shared_ptr<trace> trace2);
  static std::shared_ptr<formula> groupFormulas(const std::vector<std::shared_ptr<formula>> & formulas,
                                                                const std::string & operatorSymbol);
private:
  static bool isBooleanVariable(const std::shared_ptr<state> & state, const std::string & name);
  static int getValueOfVariable(const std::shared_ptr<state> & state, const std::string & name);

  static std::shared_ptr<formula> makeRangeFormula(std::string name, int smallestValue, int largestValue);
  static std::shared_ptr<formula> makeEqualFormula(std::string name, int value);
  static std::shared_ptr<formula> makeBooleanEqualFormula(std::string name, bool value);

};