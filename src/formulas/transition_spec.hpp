#include "../semantic/state.hpp"
#include "../semantic/transition.hpp"
#include <memory>

class transition_spec {
private:
  std::shared_ptr<state> prev_state;
  std::shared_ptr<state> next_state;
  bool is_valid = true;
  std::string prev_state_def = "";
  std::string next_state_def = "";

  void createDefintion(std::shared_ptr<state> state, std::string def)
  {
    if (def == "") {
      auto name = state->getName();
      auto formula = state->toFormula();
      def = "#define " + name + " (" + formula + ")\n";
    }
  }

public:
  // Constructor
  transition_spec(std::shared_ptr<state> prev, std::shared_ptr<state> next) : prev_state(prev), next_state(next) {}

  // Getter for prev_state
  std::shared_ptr<state> getPrevState() const { return prev_state; }

  // Setter for prev_state
  void setPrevState(std::shared_ptr<state> prev) { prev_state = prev; }

  // Getter for next_state
  std::shared_ptr<state> getNextState() const { return next_state; }

  // Setter for next_state
  void setNextState(std::shared_ptr<state> next) { next_state = next; }

  std::string getSrcDef() const
  {
    if (prev_state_def == "")
      createDefintion(prev_state, prev_state_def);
    return prev_state_def;
  }

  std::string getDstDef() const
  {
    if (next_state_def == "")
      createDefintion(next_state, next_state_def);
    return next_state_def;
  }

  std::string toFormula() const
  {
    auto negation = is_valid ? "" : "!";
    auto src_defName = getSrcDef();
    auto dst_defName = getDstDef();
    return "[](" + src_defName + " -> X(" + negation  + dst_defName + "))";
  }
};