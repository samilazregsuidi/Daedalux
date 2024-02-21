#include "trace.hpp"
#include "primitiveVariable.hpp"
#include "state.hpp"

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

trace::trace(std::vector<std::shared_ptr<transition>> transitions, std::vector<std::shared_ptr<state>> states)
{
  this->addTransitions(transitions);
  this->addStates(states);
}

trace::trace(std::vector<std::shared_ptr<state>> states) { this->addStates(states); }

trace::~trace() = default;

/// @brief This method creates a formula to discrimante between the current trace and the trace t.
/// @param t The trace to compare with.
void trace::findDistinguishingFormula(const std::shared_ptr<trace> t)
{
  auto current_trace_it = this->states.begin();
  auto other_trace_it = t->states.begin();
  std::shared_ptr<state> previous_state_current = nullptr;
  std::shared_ptr<state> previous_state_other = nullptr;
  while (current_trace_it != this->states.end() && other_trace_it != t->states.end()) {
    auto next_state_current = *current_trace_it;
    auto next_state_other = *other_trace_it;
    if (!next_state_current->isSame(next_state_other.get())) {
      // The states are different, so we need to inspect what distinguishes them.
      std::cout << "The states are different, so we need to inspect what distinguishes them." << std::endl;
      auto delta_var = next_state_current->getDelta(next_state_other.get());
      for (auto var : delta_var) {
        auto name = var->getLocalName();
        auto fullName = var->getFullName();
        auto variable_current = next_state_current->getVariable(name);
        auto variable_other = next_state_other->getVariable(name);
        auto previous_variable = previous_state_current->getVariable(name);
        const primitiveVariable * casted_variable_current = nullptr;
        const primitiveVariable * casted_variable_other = nullptr;
        const primitiveVariable * casted_previous_variable_state_1 = nullptr;
        int value_1 = 0;
        int value_2 = 0;
        int previous_value = 0;
        assert(variable_current->getType() == variable_other->getType());
        switch (variable_current->getType()) {
        case variable::V_BIT:
        case variable::V_BYTE:
        case variable::V_PID:
        case variable::V_CID:
        case variable::V_BOOL:
        case variable::V_SHORT:
        case variable::V_INT:
          casted_variable_current = dynamic_cast<const primitiveVariable *>(variable_current);
          casted_variable_other = dynamic_cast<const primitiveVariable *>(variable_other);
          casted_previous_variable_state_1 = dynamic_cast<const primitiveVariable *>(previous_variable);
          assert(casted_variable_current);
          assert(casted_variable_other);
          value_1 = casted_variable_current->getValue();
          value_2 = casted_variable_other->getValue();
          previous_value = casted_previous_variable_state_1->getValue();
          std::cout << "Variable " << name << " has value " << value_1 << " in state 1 and value " << value_2
                    << " in state 2. The previous value was " << previous_value << std::endl;
          std::cout << "Distinguishing formula: []((" << fullName << " = " << previous_value << ") ->  X(" << fullName << " = "
                    << value_1 << "))" << std::endl;
          break;
        case variable::V_MTYPE:
          break;
        case variable::V_CMTYPE:
          break;
        default:
          break;
          // assert(false);
        }
      }
    }

    // Update the previous states
    previous_state_current = next_state_current;
    current_trace_it++;
    other_trace_it++;
  }
}

bool trace::containState(const std::shared_ptr<state> s) const
{
  for (auto st : this->states) {
    if (st->delta(s.get()) < 0.000001) {
      return true;
    }
  }
  return false;
}
