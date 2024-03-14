#include "bisimulationChecker.hpp"
#include "initState.hpp"
#include "state.hpp"
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>

bool BisimulationChecker::isBisimilar(const std::shared_ptr<fsm> fsm1, const std::shared_ptr<fsm> fsm2, const TVL * tvl)
{
  // Create the initial state for both automata
  auto current_state_fsm1 = initState::createInitState(fsm1.get(), tvl);
  auto current_state_fsm2 = initState::createInitState(fsm2.get(), tvl);

  // Visited states in each LTS
  std::unordered_set<state *> visited1, visited2;

  // Queue for BFS traversal
  std::queue<state *> q1, q2;

  // Enqueue the initial states
  q1.push(current_state_fsm1);
  q2.push(current_state_fsm2);

  bool considerInternalVariables = false;

  // While the stack is not empty
  while (!q1.empty() && !q2.empty()) {
    state * current_state_fsm1 = q1.front();
    q1.pop();
    state * current_state_fsm2 = q2.front();
    q2.pop();

    auto is_same = current_state_fsm1->isSame(current_state_fsm2, considerInternalVariables);
    if (!is_same) {
      return false;
    }

    // Get the successors of the current states
    auto successors_fsm1 = current_state_fsm1->Post();
    auto successors_fsm2 = current_state_fsm2->Post();

    // Mark the current states as visited
    visited1.insert(current_state_fsm1);
    visited2.insert(current_state_fsm2);

    // Check if the current states are bisimilar
    for (auto state_fsm1 : successors_fsm1) {
      for (auto state_fsm2 : successors_fsm2) {
        bool is_found = false;
        // Check if the next states are similar
        auto same_states = state_fsm1->isSame(state_fsm2, considerInternalVariables);
        if (same_states) {
          // Check if the next states are not visited
          if (visited1.find(state_fsm1) == visited1.end() && visited2.find(state_fsm2) == visited2.end()) {
            is_found = true;
            q1.push(state_fsm1);
            q2.push(state_fsm2);
          }
        }
        if (!is_found)
          return false;
      }
    }
  }
  return true;
}

void BisimulationChecker::getDistinctStates(const std::shared_ptr<fsm> fsm1, const std::shared_ptr<fsm> fsm2)
{
  // Create the initial state for both automata
  auto states_fsm1 = fsm1->getNodes();
  auto states_fsm2 = fsm2->getNodes();
}