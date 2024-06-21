#pragma once

#include "../core/semantic/variable/state/state.hpp"
#include "../formulas/formulaUtility.hpp"
#include <string>
#include <variant>
#include <vector>

class VariableUpdateTree {
public:
  VariableUpdateTree(state * state, std::vector<VariableUpdate> updates) : st(state), updates(updates) {}
  void addChild(VariableUpdateTree child) { children.push_back(child); }

  void printTree(unsigned int level = 0) const
  {
    for (unsigned int i = 0; i < level; ++i) {
      std::cout << "  ";
    }
    for (const auto & update : updates) {
      std::cout << update.variable_name << " new value: " << VariableUpdateTree::valueType_to_string(update.new_value)
                << " old value: " << VariableUpdateTree::valueType_to_string(update.previous_value) << " | ";
    }
    std::cout << std::endl;
    for (const auto & child : children) {
      child.printTree(level + 1);
    }
  }

  std::vector<VariableUpdate> getAllUpdatesOfVariable(const std::string & variableName) const
  {
    std::vector<VariableUpdate> res;
    for (const auto & update : updates) {
      if (update.variable_name == variableName) {
        res.push_back(update);
      }
    }
    for (const auto & child : children) {
      auto childUpdates = child.getAllUpdatesOfVariable(variableName);
      res.insert(res.end(), childUpdates.begin(), childUpdates.end());
    }
    return res;
  }

  static std::string valueType_to_string(const ValueType & value)
  {
    if (std::holds_alternative<int>(value)) {
      return std::to_string(std::get<int>(value));
    }
    else if (std::holds_alternative<std::string>(value)) {
      return std::get<std::string>(value);
    }
    else { // bool case
      return std::get<bool>(value) ? "true" : "false";
    }
  }

private:
  state * st;
  std::vector<VariableUpdate> updates;
  std::vector<VariableUpdateTree> children;
};

// Class to analyze the model and generate a map of formulas
class ModelAnalyzer {
public:
  explicit ModelAnalyzer(const std::string & file_path) : file_path(file_path) {}

  void analyzeModel(void);
  void buildVariableTree(state * st, VariableUpdateTree & tree, std::vector<state *> visited_states);
  void analyzeVariableUpdates(void);

  std::string getOriginalFilePath() const { return file_path; }

private:
  const std::string & file_path;
};