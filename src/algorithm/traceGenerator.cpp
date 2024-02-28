#include "traceGenerator.hpp"
#include "explore.hpp"
#include "fsm.hpp"
#include "initState.hpp"
#include "state.hpp"
#include "transition.hpp"

/***
 * The function returns the traces as a traceReport containing both good and bad traces.
 * Parameters:
 *   no_traces - The number of traces to generate
 *   len_traces - The length of the traces
 */
std::unique_ptr<traceReport> TraceGenerator::generateTraceReport(const size_t no_traces, size_t len_traces, bool ignore_common_prefix)
{
  std::unique_ptr<traceReport> traces = std::make_unique<traceReport>();
  for (size_t i = 0; i < no_traces; ++i) {
    auto negative_trace = generateNegativeTrace(len_traces, ignore_common_prefix);
    auto positive_trace = generatePositiveTrace(len_traces, ignore_common_prefix);
    traces->addBadTrace(negative_trace);
    traces->addGoodTrace(positive_trace);
  }
  return traces;
}

//**
// * This function consumes two automata - the original automata and the mutant automata.
// * It then create a run of length @trace_length of the mutant automata that cannot be created by the original automata.
// * The function returns a list of states that represent the run.
// * Parameters:
// * 	@original - The original automata
// * 	@mutant - The mutant automata
// * 	@trace_length - The length of the run
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
  auto different_states = std::list<state *>();
  // Variables to store the current transition
  transition * current_trans_original = nullptr;
  transition * current_trans_mutant = nullptr;

  // Create a trace holding the visited states and transitions
  std::shared_ptr<trace> current_trace = std::make_shared<trace>();
  std::shared_ptr<state> current_state_mutant_copy(current_state_mutant);
  current_trace->addState(current_state_mutant_copy);
  // Keep track if the run of the two automata have the same prefix - the default is true as the initial states are the same
  bool same_prefix = true;
  size_t length = 0;

  while (length < trace_length) {
    // Check if the two nodes are the same if they have the same prefix
    post_states_original = current_state_original->Post();
    post_states_mutant = current_state_mutant->Post();
    if (post_states_mutant.empty() || post_states_original.empty()) {
      std::cout << "No more transitions to fire - the trace is complete." << std::endl;
      break;
    }
    if (same_prefix) {
      different_states = distinct_states(post_states_original, post_states_mutant);
      // If the mutant automata has a state that the original automata does not have - go to that state
      if (!different_states.empty()) {
        if (ignore_common_prefix) {
          // Add the previous state to the trace if it not already is in the trace - as it has not been added yet
          std::shared_ptr<state> curent_state_original_copy(current_state_original);
          if (current_trace->containState(curent_state_original_copy) == false) {
            current_trace->addState(curent_state_original_copy);
            std::shared_ptr<transition> current_trans_copy(current_trans_mutant);
            current_trace->addTransition(current_trans_copy);
          }
        }
        // Fire the transition
        current_state_original = different_states.front();
        // Move to the next state
        current_trans_original = current_state_original->getOrigin()->deepCopy();
        // Find the most similar transition to the fired transition in the original automata
        current_state_mutant = most_similar_state(current_state_original, post_states_mutant);
        // Apply the transition to the original automata
        current_trans_mutant = current_state_mutant->getOrigin()->deepCopy();
        // The prefix is no longer the same
        same_prefix = false;
      }
      else {
        // We could not find a different state in the mutant automata - take a random transition for both automata
        // Take the same transition as the original automata
        // Not sure if this is the best approach / if it is correct
        auto next_state_original = post_states_original.front();
        auto next_state_mutant = most_similar_state(next_state_original, post_states_mutant);
        assert(next_state_mutant->delta(next_state_original) < 0.00000001); // The states should be the same

        current_trans_mutant = next_state_mutant->getOrigin()->deepCopy();
        current_trans_original = next_state_original->getOrigin()->deepCopy();
        // Apply the transition to both automata - A hack not to use the apply function
        current_state_mutant = next_state_mutant;
        current_state_original = next_state_original;
      }
    }
    else {
      // Fire a random transition as the trace is guaranteed to be different
      current_state_mutant = post_states_mutant.front();
      auto similar_state_original = most_similar_state(current_state_mutant, post_states_original);
      current_state_original = similar_state_original;
      current_trans_mutant = current_state_mutant->getOrigin()->deepCopy();
      current_trans_original = similar_state_original->getOrigin()->deepCopy();
    }

    // Add the state and transition to the trace
    if (!ignore_common_prefix || !same_prefix) {
      std::shared_ptr<state> curent_state_mutant_copy(current_state_mutant);
      std::shared_ptr<transition> current_trans_copy(current_trans_mutant);
      current_trace->addTransition(current_trans_copy);
      current_trace->addState(curent_state_mutant_copy);
    }
    length++;
  }
  if (same_prefix)
    std::cout << "A trace in the mutant that not can be found in the original automata was not found" << std::endl;

  return current_trace;
}

//**
// * This function computes a trace that only can be be generated by the original automata, but not by the mutant automata.
// * The function returns a list of states that represent the run.
// * Parameters:
// * 	@trace_length - The length of the run
// *
std::shared_ptr<trace> TraceGenerator::generatePositiveTrace(const size_t trace_length, bool ignore_common_prefix)
{
  return generateTrace(original, mutant, trace_length, ignore_common_prefix);
}

//**
// * This function computes a trace that only can be be generated by the mutant automata, but not by the original automata.
// * The function returns a list of states that represent the run.
// * Parameters:
// * 	@trace_length - The length of the run
// *
std::shared_ptr<trace> TraceGenerator::generateNegativeTrace(const size_t trace_length, bool ignore_common_prefix)
{
  return generateTrace(mutant, original, trace_length, ignore_common_prefix);
}