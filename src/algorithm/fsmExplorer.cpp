#include "fsmExplorer.hpp"
#include <initState.hpp>
#include "../formulas/formulaCreator.hpp"
#include "utils/stateComparer.hpp"


//**
// * @brief This function generates a formula that only the original automata can satisfy!
// * Parameters:
// * 	@fsm1 - The original automata
// * 	@fsm2 - The mutant automata
// * 	@trace_length - The length of the run to generate
// * 	@ignore_common_prefix - A flag to ignore the common prefix of the two automata
// *
std::shared_ptr<formula> fsmExplorer::discardMutant(std::shared_ptr<fsm> original, std::shared_ptr<fsm> mutant)
{
  auto tvl = nullptr;
  // Create the initial state for both automata
  auto current_state_original = initState::createInitState(original.get(), tvl);
  auto current_state_mutant = initState::createInitState(mutant.get(), tvl);
  // Lists to store the transitions of the two automata
  auto post_states_original = std::list<state *>();
  auto post_states_mutant = std::list<state *>();
  auto unique_states_original = std::list<state *>();

  auto progressTraversal = [&](const std::list<state *> & successors_original, const std::list<state *> & successors_mutant) {
    if (successors_original.empty())
      throw std::runtime_error("The original automata has no successor states");
    if (successors_mutant.empty())
      throw std::runtime_error("The mutant automata has no successor states");
    // Find the next state to visit
    current_state_original = successors_original.front();
    current_state_mutant = StateComparer::most_similar_state(current_state_mutant, successors_mutant);
  };

  // Continue until we have created a formula that only the original automata can satisfy
  while (true) {
    post_states_original = current_state_original->Post();
    post_states_mutant = current_state_mutant->Post();
    if (post_states_mutant.empty() || post_states_original.empty()) {
      std::cout << "No more transitions to fire - the trace is complete." << std::endl;
      break;
    }
    // Find the states that are unique to the original automata
    unique_states_original = StateComparer::distinct_states(post_states_original, post_states_mutant);
    // The original automata has a unique state - let us continue the trace using this state
    if (!unique_states_original.empty()) {
      std::vector<std::shared_ptr<state>> post_states_original_vec;
      std::vector<std::shared_ptr<state>> post_states_mutant_vec;
      for (auto s : post_states_original) {
        post_states_original_vec.push_back(std::shared_ptr<state>(s));
      }
      for (auto s : post_states_mutant) {
        post_states_mutant_vec.push_back(std::shared_ptr<state>(s));
      }
      // We can now create a formula that only the original automata can satisfy
      auto shared_current_state_original = std::shared_ptr<state>(current_state_original);
      auto distinguishing_formula = formulaCreator::createTransitionFormula(shared_current_state_original,
                                                                            post_states_original_vec, post_states_mutant_vec);
      return distinguishing_formula;
      break;
    }
    else {
      // All the successor states are the same and the prefix is the same - take the same random transition for both
      progressTraversal(post_states_original, post_states_mutant);
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
std::map<unsigned int, std::vector<std::shared_ptr<state>>> fsmExplorer::kSuccessors(std::shared_ptr<state> start_state, unsigned int k)
{
  auto successors = std::map<unsigned int, std::vector<std::shared_ptr<state>>>();
  auto current_states = std::vector<std::shared_ptr<state>>();
  current_states.push_back(start_state);
  for (unsigned int i = 0; i < k; ++i) {
    auto next_states = std::vector<std::shared_ptr<state>>();
    for (auto s : current_states) {
      auto post_states = s->Post();
      for (auto post_state : post_states) {
        next_states.push_back(std::shared_ptr<state>(post_state));
      }
    }
    successors[i] = next_states;
    current_states = next_states;
  }
  return successors;
}
