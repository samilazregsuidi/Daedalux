#include "modelAnalyzer.hpp"
#include "../algorithm/utils/stateComparer.hpp"
#include "../core/semantic/variable/state/initState.hpp"
#include "promela_loader.hpp"
#include <memory>
#include <vector>

void ModelAnalyzer::buildVariableTree(state * st, VariableUpdateTree & tree, std::vector<state *> visited_states)
{
  if (StateComparer::containState(visited_states, st, true)) {
    return;
  }
  visited_states.push_back(st);
  auto post_states = st->SafePost();
  for (auto post_state : post_states) {
    bool shouldExplore = !StateComparer::containState(visited_states, post_state, true);
    if (!st->isSame(post_state, false)) {
      auto vars = st->getDelta(post_state, false);
      std::vector<VariableUpdate> updates;
      for (auto var : vars) {
        std::string name = var->getVisibleName();
        auto newValue = formulaUtility::getValueOfVariable(post_state, name);
        auto oldValue = formulaUtility::getValueOfVariable(st, name);
        VariableUpdate update = {name, oldValue, newValue};
        updates.push_back(update);
      }
      auto current_node = VariableUpdateTree(post_state, updates);
      if (shouldExplore) {
        buildVariableTree(post_state, current_node, visited_states);
      }
      tree.addChild(current_node);
    }
    else {
      if (shouldExplore) {
        buildVariableTree(post_state, tree, visited_states);
      }
    }
  }
}

void ModelAnalyzer::analyzeVariableUpdates(void)
{
  auto loader = new promela_loader(file_path, nullptr);
  auto fsm = loader->getAutomata();
  delete loader;
  auto start_state = initState::createInitState(fsm.get(), nullptr);
  auto visited_states = std::vector<state *>();
  auto tree = VariableUpdateTree(start_state, std::vector<VariableUpdate>());
  buildVariableTree(start_state, tree, visited_states);
  tree.printTree();
}

void ModelAnalyzer::analyzeModel(void)
{
  auto loader = new promela_loader(file_path, nullptr);
  auto fsm = loader->getAutomata();
  delete loader;
  auto start_state = initState::createInitState(fsm.get(), nullptr);
  auto visited_states = std::vector<state *>();
  auto successors = std::map<unsigned int, std::vector<state *>>();
  auto current_states = std::vector<state *>();
  current_states.push_back(start_state);
  visited_states.push_back(start_state);
  bool considerInternalVariables = false;
  unsigned int k = 15;
  VariableUpdateTree root = VariableUpdateTree(start_state, std::vector<VariableUpdate>());
  for (unsigned int i = 0; i < k; ++i) {
    auto next_states = std::vector<state *>();
    next_states.reserve(100);
    std::map<transition *, std::vector<VariableUpdate>> possibleUpdates;
    for (auto s : current_states) {
      auto post_states = s->SafePost();
      for (auto post_state : post_states) {
        if (!s->isSame(post_state, considerInternalVariables)) {
          auto vars = s->getDelta(post_state, considerInternalVariables);
          std::vector<VariableUpdate> updates;
          for (auto var : vars) {
            std::string name = var->getVisibleName();
            auto newValue = formulaUtility::getValueOfVariable(post_state, name);
            auto oldValue = formulaUtility::getValueOfVariable(s, name);
            VariableUpdate update = {name, oldValue, newValue};
            updates.push_back(update);
          }
          root.addChild(VariableUpdateTree(post_state, updates));
        }
      }
      std::copy_if(post_states.begin(), post_states.end(), std::back_inserter(next_states),
                   [&visited_states, &next_states](const auto & s) {
                     return !StateComparer::containState(visited_states, s, true) &&
                            !StateComparer::containState(next_states, s, true);
                   });
    }
    if (next_states.empty()) {
      //  No need to continue if there are no more successor states
      break;
    }
    successors[i] = next_states;
    current_states = next_states;
    visited_states.insert(visited_states.end(), next_states.begin(), next_states.end());
  }
}
