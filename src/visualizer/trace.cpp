#include "trace.hpp"

trace::trace() = default;

trace::trace(const trace & other)
{
  this->addTransitions(other.getTransitions());
  this->addStates(other.getStates());
}

trace::trace(const trace * other)
{
  this->addTransitions(other->getTransitions());
  this->addStates(other->getStates());
}

trace::trace(std::list<std::shared_ptr<transition>> transitions, std::list<std::shared_ptr<state>> states)
{
  this->addTransitions(transitions);
  this->addStates(states);
}

trace::trace(std::list<std::shared_ptr<state>> states) { this->addStates(states); }

trace::~trace() = default;

/// @brief This method creates a formula to discrimante between the current trace and the trace t.
/// @param t The trace to compare with.
void trace::discriminate(const std::shared_ptr<trace> t)
{
  auto it1 = this->states.begin();
  auto it2 = t->states.begin();
  std::shared_ptr<state> previous_state_1 = nullptr;
  std::shared_ptr<state> previous_state_2 = nullptr;
  while (it1 != this->states.end() && it2 != t->states.end()) {
    auto next_state_1 = *it1;
    auto next_state_2 = *it2;
    auto isSimilar = next_state_1->delta(next_state_2.get()) < 0.000001;
    if (!isSimilar) {
        // The states are different, so we to inspect what distinguishes them.
        std::cout << "The states are different, so we to inspect what distinguishes them." << std::endl;
        next_state_1->printDelta(next_state_2.get());
        std::cout << "State 1: " << std::endl;
        next_state_1->print();
        std::cout << "State 2: " << std::endl;
        next_state_2->print();
    }
    // Update the previous states
    previous_state_1 = next_state_1;
    previous_state_2 = next_state_2;
    it1++;
    it2++;
  }
}