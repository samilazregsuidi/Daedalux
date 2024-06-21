#include "traceGenerator.hpp"
#include "../formulas/formulaCreator.hpp"
#include "fsm.hpp"
#include "initState.hpp"
#include "state.hpp"
#include "transition.hpp"
#include "utils/stateComparer.hpp"

/***
 * The function returns the traces as a traceReport containing both good and bad traces.
 * Parameters:
 *   no_traces - The number of traces to generate
 *   len_traces - The length of the traces
 */
std::unique_ptr<traceReport> TraceGenerator::generateTraceReport(const size_t no_traces, size_t len_traces,
                                                                 bool ignore_common_prefix)
{
  std::unique_ptr<traceReport> traces = std::make_unique<traceReport>();
  for (size_t i = 0; i < no_traces; ++i) {
    auto positive_trace = generatePositiveTrace(len_traces, ignore_common_prefix);
    auto negative_trace = generateNegativeTrace(len_traces, ignore_common_prefix);
    traces->addBadTrace(negative_trace);
    traces->addGoodTrace(positive_trace);
  }
  return traces;
}

std::shared_ptr<trace> TraceGenerator::generateTrace(std::shared_ptr<fsm> original, const size_t trace_length)
{
  // Create the initial state for both automata
  auto current_state = initState::createInitState(original.get(), nullptr);

  auto result_trace = std::make_shared<trace>();
  // Lists to store post states
  auto post_states = std::list<state *>();
  std::shared_ptr<state> current_state_copy(current_state->deepCopy());
  result_trace->addState(current_state_copy);

  // Continue until we have seen the desired number of states or until we have no more transitions to fire
  while (result_trace->size() < trace_length) {
    post_states = current_state->Post();
    if (post_states.empty()) {
      std::cout << "No more transitions to fire - the trace is complete." << std::endl;
      break;
    }
    // Take a random successor state
    auto random_index = rand() % post_states.size();
    auto post_states_it = post_states.begin();
    std::advance(post_states_it, random_index);
    current_state = *post_states_it;
    auto trans = current_state->getOrigin()->deepCopy();
    // Add the state and transition to the trace
    std::shared_ptr<state> state_copy(current_state);
    std::shared_ptr<transition> transition_copy(trans);
    result_trace->addState(state_copy);
    result_trace->addTransition(transition_copy);
  }
  return result_trace;
}

//**
// * @brief This function generates a trace of length @trace_length that only fsm1 can generate, but not fsm2.
// * Parameters:
// * 	@fsm1 - The original automata
// * 	@fsm2 - The mutant automata
// * 	@trace_length - The length of the run to generate
// * 	@ignore_common_prefix - A flag to ignore the common prefix of the two automata
// *
std::shared_ptr<trace> TraceGenerator::generateTrace(std::shared_ptr<fsm> original, std::shared_ptr<fsm> mutant,
                                                     const size_t trace_length, bool ignore_common_prefix)
{
  // Create the initial state for both automata
  auto current_state_original = initState::createInitState(original.get(), tvl);
  auto current_state_mutant = initState::createInitState(mutant.get(), tvl);
  // Lists to store the transitions of the two automata
  auto post_states_original = std::list<state *>();
  auto post_states_mutant = std::list<state *>();
  // Variables to store the current transition
  transition * current_trans_original = nullptr;
  transition * current_trans_mutant = nullptr;

  // Create a trace holding the visited states and transitions
  std::shared_ptr<trace> result_trace = std::make_shared<trace>();
  std::shared_ptr<state> current_state_mutant_copy(current_state_mutant);
  result_trace->addState(current_state_mutant_copy);
  // Keep track if the run of the two automata have the same prefix - the default is true as the initial states are the same
  bool same_prefix = true;
  size_t length = 0;

  auto progressTraversal = [&](const std::list<state *> & successors_original, const std::list<state *> & successors_mutant) {
    if (successors_original.empty())
      throw std::runtime_error("The original automata has no successor states");
    if (successors_mutant.empty())
      throw std::runtime_error("The mutant automata has no successor states");
    // Find the next state to visit
    current_state_original = successors_original.front();
    current_state_mutant = StateComparer::most_similar_state(current_state_mutant, successors_mutant);
    // Find the transition that leads to the next state
    current_trans_mutant = current_state_original->getOrigin()->deepCopy();
    current_trans_original = current_state_mutant->getOrigin()->deepCopy();
  };

  // Continue until we have seen the desired number of states or until we have no more transitions to fire
  while (length < trace_length) {
    post_states_original = current_state_original->Post();
    post_states_mutant = current_state_mutant->Post();
    if (post_states_mutant.empty() || post_states_original.empty()) {
      std::cout << "No more transitions to fire - the trace is complete." << std::endl;
      break;
    }
    if (same_prefix) {
      // Find the states that are unique to the original automata
      auto unique_states_original = StateComparer::distinct_states(post_states_original, post_states_mutant);
      // The original automata has a unique state - let us continue the trace using this state
      if (!unique_states_original.empty()) {
        if (ignore_common_prefix) {
          // Since we are ignoring the common prefix, we need to add the previous state to the trace
          std::shared_ptr<state> curent_state_original_copy(current_state_original);
          if (result_trace->containState(curent_state_original_copy) == false) {
            result_trace->addState(curent_state_original_copy);
          }
        }
        // Continue the trace using the unique state
        progressTraversal(unique_states_original, post_states_mutant);
        same_prefix = false;
      }
      else {
        // All the successor states are the same and the prefix is the same - take the same random transition for both
        progressTraversal(post_states_original, post_states_mutant);
      }
    }
    else {
      // Fire a random transition - the trace is guaranteed to be different
      progressTraversal(post_states_original, post_states_mutant);
    }

    // Add the state and transition to the trace
    if (!ignore_common_prefix || !same_prefix) {
      std::shared_ptr<state> state_copy(current_state_original);
      std::shared_ptr<transition> transition_copy(current_trans_original);
      result_trace->addState(state_copy);
      result_trace->addTransition(transition_copy);
    }
    // Increase the length of the walk
    length++;
  }
  if (same_prefix)
    std::cout << "A trace in the mutant that not can be found in the original automata was not found" << std::endl;

  return result_trace;
}

//**
// * @brief This function generates a trace of length @trace_length that only fsm1 can generate, but not fsm2.
// * Parameters:
// * 	@original - The original automata
// * 	@mutant - The mutant automata
// * 	@trace_length - The length of the run to generate
// *
std::shared_ptr<trace> TraceGenerator::generateDiscriminatingTrace(std::shared_ptr<fsm> original, std::shared_ptr<fsm> mutant,
                                                                   const size_t trace_length, bool appendLastStateIfDeadlock)
{
  // Create the initial state for both automata
  auto current_state_original = initState::createInitState(original.get(), tvl);
  auto current_state_mutant = initState::createInitState(mutant.get(), tvl);
  // Lists to store the transitions of the two automata
  auto post_states_original = std::list<state *>();
  auto post_states_mutant = std::list<state *>();
  // Variables to store the current transition
  transition * current_trans_original = nullptr;
  transition * current_trans_mutant = nullptr;

  // Create a trace holding the visited states and transitions
  std::shared_ptr<trace> result_trace = std::make_shared<trace>();
  std::shared_ptr<state> current_state_original_copy(current_state_original);
  result_trace->addState(current_state_original_copy);
  // Keep track if the run of the two automata have the same prefix - the default is true as the initial states are the same
  bool is_different = false;

  auto progressTraversal = [&](const std::list<state *> & successors_original, const std::list<state *> & successors_mutant) {
    // Find a random successor state
    std::vector<state *> post_states_original(successors_original.begin(), successors_original.end());
    auto random_index = rand() % post_states_original.size();
    current_state_original = post_states_original[random_index];
    if (!successors_mutant.empty()) {
      current_state_mutant = StateComparer::most_similar_state(current_state_mutant, successors_mutant);
      // Find the transition that leads to the next state
      current_trans_mutant = current_state_original->getOrigin()->deepCopy();
      current_trans_original = current_state_mutant->getOrigin()->deepCopy();
    }
  };

  // Continue until we have seen the desired number of states or until we have no more transitions to fire
  while (result_trace->size() < trace_length) {
    post_states_original = current_state_original->SafePost();
    post_states_mutant = current_state_mutant->SafePost();
    if (post_states_original.empty()) {
      if (appendLastStateIfDeadlock) {
        while (result_trace->size() < trace_length) {
          auto state_copy = current_state_original->deepCopy();
          auto shared_state = std::shared_ptr<state>(state_copy);
          result_trace->addState(shared_state);
        }
      }
      break;
    }
    // Find the states that are unique to the original automata
    auto unique_states_original = StateComparer::distinct_states(post_states_original, post_states_mutant);
    auto post_states = unique_states_original.empty() ? post_states_original : unique_states_original;
    progressTraversal(post_states, post_states_mutant);
    is_different = is_different || !unique_states_original.empty();

    // Add the state and transition to the trace
    std::shared_ptr<state> state_copy(current_state_original);
    result_trace->addState(state_copy);
  }
  result_trace->setDifferentiatingTrace(is_different);
  return result_trace;
}

//**
// * This function computes a trace that only can be be generated by the original automata, but not by the mutant automata.
// * The function returns a list of states that represent the run.
// * Parameters:
// * 	@trace_length - The length of the run
// *
std::shared_ptr<trace> TraceGenerator::generatePositiveTrace(const size_t trace_length, bool appendLastStateIfDeadlock,
                                                             bool ignore_common_prefix)
{
  return generateDiscriminatingTrace(original, mutant, trace_length, appendLastStateIfDeadlock);
}

//**
// * This function computes a trace that only can be be generated by the mutant automata, but not by the original automata.
// * The function returns a list of states that represent the run.
// * Parameters:
// * 	@trace_length - The length of the run
// *
std::shared_ptr<trace> TraceGenerator::generateNegativeTrace(const size_t trace_length, bool appendLastStateIfDeadlock,
                                                             bool ignore_common_prefix)
{
  return generateDiscriminatingTrace(mutant, original, trace_length, appendLastStateIfDeadlock);
}