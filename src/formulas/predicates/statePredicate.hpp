#pragma once
#include <memory>
#include <string>
#include "../state/state.hpp"

class statePredicate {
public:
    statePredicate(std::string name) : name(name) {}

    virtual ~statePredicate() = default;

    virtual bool isSatisfied(const std::shared_ptr<state> s) const = 0;

    std::string getName () const { return name; }

    virtual bool operator==(const statePredicate& other) const = 0;

    virtual std::size_t hash() const = 0;

private:
    std::string name;
};

struct statePredicateHash {
    std::size_t operator()(const std::shared_ptr<statePredicate>& obj) const {
        return obj->hash();
    }
};

struct statePredicateEqual {
    bool operator()(const std::shared_ptr<statePredicate>& lhs, const std::shared_ptr<statePredicate>& rhs) const {
        return *lhs == *rhs;
    }
};