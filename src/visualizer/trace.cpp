#include "trace.hpp"
#include "scalarVarInt.hpp"
#include "state.hpp"
#include <numeric>

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

std::string trace::projectTrace(const std::vector<std::shared_ptr<statePredicate>> & predicates)
{
  // Using accumulate to build the results string with conditions
  std::string resultString = std::accumulate(this->states.begin(), this->states.end(), std::string{},
      [&](const std::string& acc, const std::shared_ptr<state>& s) {
          std::string line = std::accumulate(predicates.begin(), predicates.end(), std::string{},
              [&](const std::string& lineAcc, const std::shared_ptr<statePredicate>& p) {
                  return lineAcc + (!lineAcc.empty() ? ", " : "") + (p->isSatisfied(s) ? "1" : "0");
              });
          return acc + (!acc.empty() ? "; " : "") + line;
      });
  return resultString;
}

/// @brief A method for extracting all the predicates from the states of the trace
/// @return A vector of shared pointers to the statePredicate objects
std::vector<std::shared_ptr<statePredicate>> trace::getPredicates() const{
  std::vector<std::shared_ptr<statePredicate>> result;
  for (auto st : this->states) {
    auto predicates = st->getPredicates();
    for (auto pred : predicates) {
      result.push_back(pred);
    }
  }
  return result;
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