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

bool trace::containState(const std::shared_ptr<state> s) const
{
  bool considerInternalVariables = true;
  for (auto st : this->states) {
    if (st->isSame(s.get(), considerInternalVariables)) {
      return true;
    }
  }
  return false;
}

// std::map<std::string, std::pair<int, int>> trace::getMinMaxValues() const
// {
//   std::map<std::string, std::pair<int, int>> result;
//   for (auto st : this->states) {
//     auto variables = st->getVariables();
//     for (auto var : variables) {
//       auto name = var->getFullName();
//       auto value = var->second->getValue();
//       if (result.find(name) == result.end()) {
//         result[name] = std::pair<int, int>(value, value);
//       } else {
//         auto current = result[name];
//         if (value < current.first) {
//           current.first = value;
//         }
//         if (value > current.second) {
//           current.second = value;
//         }
//         result[name] = current;
//       }
//     }
//   }
//   return result;
// }