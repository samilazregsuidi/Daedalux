#pragma once

#include "state.hpp"
#include "trace.hpp"
#include <memory>
#include <string>
#include <vector>

class formulaCreator {
public:
  static std::string distinguishStates(const std::shared_ptr<state> & state1, const std::shared_ptr<state> & state2);
  static void groupStates(const std::vector<std::shared_ptr<state>> & states);
  static std::unordered_map<std::string, std::map<int, std::vector<std::shared_ptr<state>>>>
  buildVariableValueMap(const std::vector<std::shared_ptr<state>> & states);
  static std::string distinguishTraces(const std::shared_ptr<trace> & include_trace,
                                       const std::shared_ptr<trace> & exclude_trace);
  static void distinguishStates(const std::vector<std::shared_ptr<state>> include_states,
                                const std::vector<std::shared_ptr<state>> exclude_states);

  static std::pair<std::shared_ptr<trace>, std::shared_ptr<trace>> removeCommonPrefixes(const std::shared_ptr<trace> trace1,
                                                                                        const std::shared_ptr<trace> trace2);

private:
  static int getValueOfVariable(const std::shared_ptr<state> & state, const std::string & name);
};