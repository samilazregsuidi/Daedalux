#include <memory>
#include "../semantic/state.hpp"
#include "../semantic/transition.hpp"

class transition_spec {
private:
    std::shared_ptr<state> prev_state;
    std::shared_ptr<state> next_state;
    bool is_valid;

public:
    // Constructor
    transition_spec(std::shared_ptr<state> prev, std::shared_ptr<state> next)
        : prev_state(prev), next_state(next) {
    }

    // Getter for prev_state
    std::shared_ptr<state> getPrevState() const {
        return prev_state;
    }

    // Setter for prev_state
    void setPrevState(std::shared_ptr<state> prev) {
        prev_state = prev;
    }

    // Getter for next_state
    std::shared_ptr<state> getNextState() const {
        return next_state;
    }

    // Setter for next_state
    void setNextState(std::shared_ptr<state> next) {
        next_state = next;
    }

    std::string toString() const {
        return prev_state->getName() + " -> " + next_state->getName();
    }

    std::string toFormula() const {
        if (is_valid)
            return "G(" + prev_state->getName() + " -> X(" + next_state->getName() + "))";
        else
            return "G(" + prev_state->getName() + " -> X(!" + next_state->getName() + "))";
    }
};