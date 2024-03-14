#include "fsmExplorer.hpp"
#include "../formulas/formulaCreator.hpp"
#include "utils/stateComparer.hpp"
#include <initState.hpp>

/// @brief Given a state, this function returns the successor states of the state, but avoids epsilon transitions where only
/// internal variables are updated
/// @param start_state - The state to find the successor states of
/// @param budget - The number of steps to maximally take
/// @return A list of the successor states of the state
std::list<state *> fsmExplorer::avoidEpsilonSteps(state * start_state, unsigned int budget)
{
  if (budget == 0) {
    // We have reached the budget - return an empty list
    return std::list<state *>();
  }
  auto post_states = start_state->Post();
  auto post_states_no_epsilon = std::list<state *>();
  auto epsilon_states = std::list<state *>();
  bool considerInternalVariables = false;
  for (auto s : post_states) {
    if (s->isSame(start_state, considerInternalVariables)) {
      epsilon_states.push_back(s);
    }
    else {
      post_states_no_epsilon.push_back(s);
    }
  }
  if (post_states_no_epsilon.empty()) {
    for (auto s : epsilon_states) {
      auto next_states = avoidEpsilonSteps(s, budget - 1);
      for (auto next_state : next_states) {
        post_states_no_epsilon.push_back(next_state);
      }
    }
  }
  return post_states_no_epsilon;
}

//**
// * @brief This function generates a formula that only the original automata can satisfy!
// * Parameters:
// * 	@fsm1 - The original automata
// * 	@fsm2 - The mutant automata
// * @return A formula that only the original automata can satisfy
std::shared_ptr<formula> fsmExplorer::discardMutant(std::shared_ptr<fsm> original, std::shared_ptr<fsm> mutant)
{
  auto tvl = nullptr;
  // Create the initial state for both automata
  auto current_state_original = initState::createInitState(original.get(), tvl);
  auto current_state_mutant = initState::createInitState(mutant.get(), tvl);
  // Lists to store the  post states of the original
  auto post_states_original = std::list<state *>();
  // Lists to store the  post states of the mutant
  auto post_states_mutant = std::list<state *>();
  auto unique_states_original = std::list<state *>();

  auto visited_states_original = std::vector<state *>();
  auto visited_states_mutant = std::vector<state *>();

  bool considerInternalVariables = true;

  auto progressTraversal = [&](const std::list<state *> & successors_original, const std::list<state *> & successors_mutant) {
    // Add the current state to the list of visited states
    visited_states_original.push_back(current_state_original);
    visited_states_mutant.push_back(current_state_mutant);
    if (successors_original.empty())
      throw std::runtime_error("The original automata has no successor states");
    if (successors_mutant.empty())
      throw std::runtime_error("The mutant automata has no successor states");
    // Find the next state to visit
    current_state_original = successors_original.front();
    current_state_mutant = StateComparer::most_similar_state(current_state_original, successors_mutant);
  };

  // Continue until we have created a formula that only the original automata can satisfy
  while (true) {
    post_states_original = current_state_original->SafePost();
    post_states_mutant = current_state_mutant->SafePost();

    // Find the states that are unique to the original automata
    unique_states_original = StateComparer::distinct_states(post_states_original, post_states_mutant);
    // The original automata has a unique state - let us continue the trace using this state
    if (!unique_states_original.empty()) {
      std::vector<std::shared_ptr<state>> post_states_original_vec;
      std::vector<std::shared_ptr<state>> post_states_mutant_vec;
      if (post_states_mutant.empty()) {
        // We need to make sure that the original automata has a distinct successor state
        bool found = false;
        for (auto s : unique_states_original) {
          if (!s->isSame(current_state_original, false)) {
            found = true;
            break;
          }
        }
        if (!found) {
          std::cout << "The current state is no different from its successor states" << std::endl;
          // We need to find a successor state that is not the same as the current state
        }
      }
      for (auto s : post_states_original) {
        post_states_original_vec.push_back(std::shared_ptr<state>(s));
      }
      for (auto s : post_states_mutant) {
        post_states_mutant_vec.push_back(std::shared_ptr<state>(s));
      }
      auto k = 15;
      // We can now create a formula that only the original automata can satisfy
      auto shared_current_state_original = std::shared_ptr<state>(current_state_original);
      auto shared_current_state_mutant = std::shared_ptr<state>(current_state_mutant);
      auto kSuccessors_original = kSuccessors(current_state_original, k);
      auto kSuccessors_mutant = kSuccessors(current_state_mutant, k);
      auto comparison = StateComparer::compareKSuccessors(kSuccessors_original, kSuccessors_mutant);

      auto distinguishing_formula = formulaCreator::createTransitionFormula(shared_current_state_original,
                                                                            post_states_original_vec, post_states_mutant_vec);

      auto states_original = comparison.getOriginalStates();
      auto last_state_original = states_original[states_original.size() - 1];
      last_state_original->print();
      auto states_mutant = comparison.getMutantStates();
      auto last_state_mutant = states_mutant[states_mutant.size() - 1];
      last_state_mutant->print();
      //   auto distinguishing_formula_2 = formulaCreator::distinguishStates(states_original, states_mutant);

      std::cout << "The distinguishing formula is " << distinguishing_formula->toFormula() << std::endl;
      return distinguishing_formula;
    }
    else {
      // All the successor states are the same and the prefix is the same - take the same random transition for both
      progressTraversal(post_states_original, post_states_mutant);
      if (StateComparer::containState(visited_states_original, current_state_original, considerInternalVariables) &&
          StateComparer::containState(visited_states_mutant, current_state_mutant, considerInternalVariables)) {
        std::cout << "The current state is already visited" << std::endl;
        break;
      }
    }
  }
  return std::make_shared<BooleanConstant>(true);
}

//**
// * This function computes the k-successors of a state
// * The function returns a map of the k-successors of the state
// * Parameters:
// * 	@start_state - The state to compute the k-successors of
// * 	@k - The number of successors to compute
// *
std::map<unsigned int, std::vector<state *>> fsmExplorer::kSuccessors(state * start_state, unsigned int k)
{
  auto successors = std::map<unsigned int, std::vector<state *>>();
  auto current_states = std::vector<state *>();
  current_states.push_back(start_state);
  for (unsigned int i = 0; i < k; ++i) {
    auto next_states = std::vector<state *>();
    for (auto s : current_states) {
      auto post_states = s->SafePost();
      for (auto post_state : post_states) {
        if (StateComparer::containState(next_states, post_state, false)) {
          // If the state is already in the list of next states, we do not need to add it again
          continue;
        }
        next_states.push_back(post_state);
      }
    }
    successors[i] = next_states;
    current_states = next_states;
  }
  return successors;
}
