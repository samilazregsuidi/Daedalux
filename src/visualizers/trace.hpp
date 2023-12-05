#pragma once

#include <iostream>
#include <list>
#include <memory> // Include for smart pointers
#include <string>

#include "state.hpp"
#include "transition.hpp"

class trace {
private:
  std::list<std::shared_ptr<transition>> transitions;
  std::list<std::shared_ptr<state>> states;

public:
  trace();
  ~trace();

  std::list<std::shared_ptr<transition>> getTransitions() const { return this->transitions; }
  std::list<std::shared_ptr<state>> getStates() const { return this->states; }

  void addTransition(std::shared_ptr<transition> t) { this->transitions.push_back(t); }
  void addState(std::shared_ptr<state> s) { this->states.push_back(s); }

  size_t size() const { return this->transitions.size(); }

  void addTransitions(const std::list<std::shared_ptr<transition>> & Ts)
  {
    for (auto t : Ts) {
      this->addTransition(t);
    }
  }
  void addStates(const std::list<std::shared_ptr<state>> & Ss)
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

  friend bool operator==(const trace & lhs, const trace & rhs)
  {
    return (lhs.getTransitions() == rhs.getTransitions()) && (lhs.getStates() == rhs.getStates());
  }

  friend bool operator!=(const trace & lhs, const trace & rhs) { return !(lhs == rhs); }
};

template <typename... Args> size_t hash_all(const Args &... args)
{
  uint64_t hash = 0xc3a5c85c97cb3127ULL;
  auto hash_mix = [&hash](uint64_t v) {
    hash += v;
    hash ^= hash >> 33;
    hash *= 0xff51afd7ed558ccdULL;
    hash ^= hash >> 33;
    hash *= 0xc4ceb9fe1a85ec53ULL;
    hash ^= hash >> 33;
  };
  (hash_mix(std::hash<Args>{}(args)), ...);
  return hash;
}

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