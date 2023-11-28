#pragma once

#include <iostream>
#include <list>
#include <string>

#include "state.hpp"
#include "transition.hpp"

class trace {
private:
  std::list<transition *> transitions;
  std::list<state *> states;
  // automata* automaton;

public:
  trace();
  ~trace();

  std::list<transition *> getTransitions() const { return this->transitions; }
  std::list<state *> getStates() const { return this->states; }

  void addTransition(transition * t) { this->transitions.push_back(t); }
  void addState(state * s) { this->states.push_back(s); }

  size_t size() const { return this->transitions.size(); }

  void addTransitions(const std::list<transition *> & Ts)
  {
    for (auto t : Ts) {
      this->addTransition(t);
    }
  }
  void addStates(const std::list<state *> & Ss)
  {
    for (auto s : Ss) {
      this->addState(s);
    }
  }

  void addTrace(const trace * other)
  {
    this->addTransitions(other->getTransitions());
    this->addStates(other->getStates());
  }

  void printCSV(std::ostream & out) const
  {
    auto start = this->states.front();
    start->printCSVHeader(out);
    for (auto st : this->states) {
      st->printCSV(out);
    }
  }

  // Define the equality (==) operator
  friend bool operator==(const trace & lhs, const trace & rhs)
  {
    return (lhs.getTransitions() == rhs.getTransitions()) && (lhs.getStates() == rhs.getStates());
  }

  // Define the inequality (!=) operator
  friend bool operator!=(const trace & lhs, const trace & rhs) { return !(lhs == rhs); }
};

// A simple hash combiner used to hash multiple values together
// Hashes all args and mixes their hashed bits together
template <typename... Args> size_t hash_all(const Args &... args)
{
  uint64_t hash = 0xc3a5c85c97cb3127ULL; // A large prime number
  // Create a lambda for mixing 64bit hashes
  // https://cppbyexample.com/lambdas.html
  auto hash_mix = [&hash](uint64_t v) {
    hash += v;
    // fmix64 from MurmurHash
    hash ^= hash >> 33;
    hash *= 0xff51afd7ed558ccdULL;
    hash ^= hash >> 33;
    hash *= 0xc4ceb9fe1a85ec53ULL;
    hash ^= hash >> 33;
  };
  // For each arg hash it with std::hash and mix it with hash_mix
  (hash_mix(std::hash<Args>{}(args)), ...);
  return hash;
}

// Our custom std::hash specialization for Trace
template <> struct std::hash<trace> {
  size_t operator()(const trace & t) const noexcept
  {
    auto transitions = t.getTransitions();
    uint64_t hash = 0;
    for (auto t : transitions) {
      hash = hash_all(t->src->hash(), t->dst->hash(), t->action, t->prob, hash);
    }
    for (auto s : t.getStates()) {
      hash = hash_all(s, hash);
    }
    return hash;
  }
};