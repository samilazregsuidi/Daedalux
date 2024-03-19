#include "fsmExplorer.hpp"
#include "../formulas/formulaCreator.hpp"
#include "utils/stateComparer.hpp"

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
      std::copy(next_states.begin(), next_states.end(), std::back_inserter(post_states_no_epsilon));
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
    auto unique_states_original = StateComparer::distinct_states(post_states_original, post_states_mutant);
    // The original automata has a unique state - let us continue the trace using this state
    if (!unique_states_original.empty()) {

      if (post_states_mutant.empty()) {
        // We need to make sure that the original automata has a distinct successor state
        bool found = StateComparer::containState(unique_states_original, current_state_original, false);
        if (!found) {
          std::cout << "The current state is no different from its successor states" << std::endl;
          // We need to find a successor state that is not the same as the current state
        }
      }
      std::vector<std::shared_ptr<state>> post_states_original_vec;
      std::vector<std::shared_ptr<state>> post_states_mutant_vec;
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

      analyzeSuccessors(current_state_original, current_state_mutant, k);

      auto distinguishing_formula = formulaCreator::createTransitionFormula(shared_current_state_original,
                                                                            post_states_original_vec, post_states_mutant_vec);

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

/// @brief This function analyzes the k-successors of two states from two automata
/// @param state_original the state from the original automata
/// @param state_mutant the state from the mutant automata
/// @param k the number of successors to analyze
void fsmExplorer::analyzeSuccessors(state * state_original, state * state_mutant, unsigned int k)
{
  auto kSuccessors_original = kSuccessors(state_original, k);
  auto kSuccessors_mutant = kSuccessors(state_mutant, k);

  auto comparison = StateComparer::compareKSuccessors(kSuccessors_original, kSuccessors_mutant);

  auto formula_original = kSuccessors_original.generateFormula();
  auto formula_mutant = kSuccessors_mutant.generateFormula();

  std::cout << "The formula for the original automata is " << formula_original->toFormula() << std::endl;
  std::cout << "The formula for the mutant automata is " << formula_mutant->toFormula() << std::endl;

  std::cout << "The comparison is " << std::endl;
  //auto stateFormula = formulaCreator::groupStatesByFormula({most_different_ptr});
}

//**
// * This function computes the k-successors of a state
// * The function returns a map of the k-successors of the state
// * Parameters:
// * 	@start_state - The state to compute the k-successors of
// * 	@k - The number of successors to compute
// *
successorTree fsmExplorer::kSuccessors(state * start_state, unsigned int k)
{
  auto visited_states = std::vector<state *>();
  auto successors = std::map<unsigned int, std::vector<state *>>();
  auto current_states = std::vector<state *>();
  current_states.push_back(start_state);
  visited_states.push_back(start_state);
  bool considerInternalVariables = true;
  for (unsigned int i = 0; i < k; ++i) {
    auto next_states = std::vector<state *>();
    next_states.reserve(100);
    for (auto s : current_states) {
      auto post_states = s->SafePost();
      std::copy_if(post_states.begin(), post_states.end(), std::back_inserter(next_states),
                   [&visited_states, &considerInternalVariables, &next_states](const auto & s) {
                     return !StateComparer::containState(visited_states, s, considerInternalVariables) &&
                            !StateComparer::containState(next_states, s, considerInternalVariables);
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
  auto successor_tree = successorTree(successors);
  return successor_tree;
}
