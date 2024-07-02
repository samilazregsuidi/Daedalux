#include "explore.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>

#include "initState.hpp"
#include "process.hpp"
#include "semantic.hpp"
#include "transition.hpp"

#define PRINT_STATE print

#define B 5

stateToGraphViz * graphVis = nullptr;

// TODO: Do we this method?
void launchExecution(const fsm * automata, const TVL * tvl)
{
  state * current = initState::createInitState(automata, tvl);
  unsigned long i = 0;
  // printf("**********************************\n");
  current->PRINT_STATE();
  graphVis = new stateToGraphViz(automata);
  graphVis->printGraphViz(current);

  while (transition * trans = transition::sampleUniform(current->executables())) {
    ++i;
    current->apply(trans);
    // printf("--------------------------------------\n");
    current->PRINT_STATE();
    graphVis->printGraphViz(current);

    if (current->isAccepting())
      printf("***** ACCEPTING STATE *****\n");

    if (i >= B) {
      break;
    }
    // add error status
  }
  printf("--\n");
}

std::unique_ptr<trace> interactiveDebugging(const std::shared_ptr<fsm>& automata, const size_t trace_length, const TVL * tvl)
{
  // Create the initial state for both automata
  auto current_state = initState::createInitState(automata.get(), tvl);
  // Lists to store the transitions of the two automata
  auto post_states = std::list<state *>();

  // Create a trace holding the visited states and transitions
  std::unique_ptr<trace> current_trace = std::make_unique<trace>();
  std::shared_ptr<state> current_state_copy(current_state);
  current_trace->addState(current_state_copy);

  while (current_trace->size() < trace_length) {
    post_states = current_state->Post();
    if (post_states.empty()) {
      std::cout << "No more transitions to fire - the trace is complete." << std::endl;
      break;
    }
    std::cout << "**********************************" << std::endl;
    std::cout << "Current state" << std::endl;
    current_state->PRINT_STATE();
    std::cout << "Choose a transition to fire by selecting a number between 0 and " << post_states.size() << "." << std::endl;
    std::cout
        << "The transitions are displayed as \"new value -> current value\" based on how they will change the current state."
        << std::endl;
    int index = 0;
    bool considerInternalVariables = true;
    for (auto & p : post_states) {
      std::cout << index << " : ";
      p->printDelta(current_state, considerInternalVariables);
      index++;
    }
    auto post_state_vector = std::vector<state *>(post_states.begin(), post_states.end());
    int choice;
    std::cin >> choice;
    if (choice < 0 || choice >= ((int)post_states.size())) {
      std::cout << "Invalid choice - firing the first transition." << std::endl;
      choice = 0;
    }
    current_state = post_state_vector[choice];
    transition * trans = const_cast<transition *>(current_state->getOrigin());
    std::shared_ptr<transition> current_trans_copy(trans);
    std::shared_ptr<state> curent_state_copy(current_state);
    current_trace->addTransition(current_trans_copy);
    current_trace->addState(curent_state_copy);
  }
  return current_trace;
}

void fireNonActionTransition(state * current)
{
  transition * trans = nullptr;
  while (true) {
    auto transitions = current->executables();
    trans = transition::sampleNonUniform(transitions);
    if (trans->action == "")
      current->apply(trans);
    else
      break;
  }
}

int launchExecutionMarkovChain(const fsm * automata, const TVL * tvl)
{
  state * current = initState::createInitState(automata, tvl);
  fireNonActionTransition(current);
  unsigned long i = 0;
  // printf("**********************************\n");
  current->PRINT_STATE();

  graphVis = new stateToGraphViz(automata);
  graphVis->printGraphViz(current);
  std::vector<std::string> scheduler;

  std::ifstream myfile;

  // Should this file be hard-coded?
  myfile.open("mdp.sched");

  std::string line;

  if (myfile.is_open()) {
    while (myfile) { // equivalent to myfile.good()
      std::getline(myfile, line);
      scheduler.push_back(line);
      std::cout << line << '\n';
    }
  }
  else {
    std::cerr << "Couldn't open file\n";
  }

  transition * trans = nullptr;

  do {
    int sValue = current->getValue<byteVar*>("s");
    std::string schedValue = scheduler[sValue];

    auto transitions = current->executables();
    trans = transition::select(transitions, schedValue);
    assert(trans->action != "");
    current->apply(trans);

    fireNonActionTransition(current);

    // printf("--------------------------------------\n");
    // if(i % 3 == 0) {
    current->PRINT_STATE();
    graphVis->printGraphViz(current);
    //}

    if (current->isAccepting()) {
      std::cout << "accepting trace" << std::endl;
      return 0;
    }

    ++i;
    if (i >= B) {
      break;
    }
    // add error status
  } while (trans);

  printf("--\n");

  std::cout << "non accepting trace" << std::endl;
  return 1;
}

#define K 3

/**
 * This function tries to find a lasso in the state space of a given finite state machine using a random walk.
 * @param automata A pointer to the finite state machine to create the state space for.
 * @param tvl A pointer to the transition vector list for the finite state machine.
 * @param k_steps The number of steps to take in the random walk.
 */
void findLasso(const fsm * automata, const TVL * tvl, size_t k_steps)
{
  std::set<unsigned long> hashSet;
  state * current = initState::createInitState(automata, tvl);
  transition * trans = nullptr;
  for (size_t i = 0; i < k_steps; ++i) {
    // Print current state and visualize it
    // printf("**********************************\n");
    current->PRINT_STATE();
    graphVis->printGraphViz(current);
    auto hash = current->hash();
    bool isNewState = (hashSet.find(hash) == hashSet.end());
    if (isNewState) {
      hashSet.insert(hash);
      // Sample a uniform transition and apply it
      trans = transition::sampleUniform(current->executables());
      if (trans) {
        printf("..\n");
        current->apply(trans);
      }
      else {
        break; // No valid transition, exit the loop
      }
    }
    else {
      break; // Detected a lasso, exit the loop
    }
  }
  printf("--\n");
}

#define D 1000
/**
 * This function prints the state space for a given finite state machine using breath-first search to the console.
 *
 * @param automata A pointer to the finite state machine to create the state space for.
 * @param tvl A pointer to the transition vector list for the finite state machine.
 */
void createStateSpaceBFS(const fsm * automata, const TVL * tvl)
{
  elementStack st;
  std::set<unsigned long> hm;
  std::shared_ptr<state> init(initState::createInitState(automata, tvl));

  graphVis = new stateToGraphViz(automata);
  graphVis->printGraphViz(init.get());

  int depth = 0;

  st.push(init, depth);
  hm.insert(init->hash());

  unsigned long i = 0;

  while (!st.empty()) {
    auto current = st.top();
    auto current_state = current->current_state;
    i++;
    printf("****************** current state ****************\n");
    current_state->PRINT_STATE();
    st.pop();
    depth = current->depth;

    auto successors = current_state->Post();
    // delete current;

    if (successors.size() > 0) {
      printf("************* next possible states **************\n");
      ++depth;
      for (auto & n : successors) {

        n->PRINT_STATE();

        if (hm.find(n->hash()) != hm.end()) {
          printf("************* already visited state **************\n");
          // delete n;
        }
        else {
          std::shared_ptr<state> n_(n);
          st.push(n_, depth);
          hm.insert(n->hash());
          graphVis->printGraphViz(n, depth);
        }

        if (successors.size() > 1) {
          printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
        }
      }
    }
    else {
      printf("************* end state **************\n");
    }
  }

  printf("number of states : %lu\n", i);
  delete graphVis;
}

/**
 * This function prints the state space for a given finite state machine using depth-first search to the console.
 * @param automata A pointer to the finite state machine to create the state space for.
 * @param tvl A pointer to the transition vector list for the finite state machine.
 */
void createStateSpaceDFS(const fsm * automata, const TVL * tvl)
{
  elementStack st;
  std::set<unsigned long> hm;
  std::shared_ptr<state> init(initState::createInitState(automata, tvl));

  graphVis = new stateToGraphViz(automata);

  int depth = 0;

  st.push(init, depth);
  hm.insert(init->hash());

  unsigned long i = 0;

  while (!st.empty()) {
    auto current = st.top();
    auto current_state = current->current_state;

    printf("****************** current state ****************\n");
    current_state->PRINT_STATE();
    if (!current->init) {
      std::list<std::shared_ptr<state>> sPost_;
      for (auto & p : current_state->Post()) {
        std::shared_ptr<state> postState(p);
        sPost_.push_back(postState);
      }
      current->Post = sPost_;
      current->init = true;
    }

    if (!current->Post.empty()) {
      auto n = *current->Post.begin();
      current->Post.pop_front();

      printf("************* pick next state **************\n");

      n->PRINT_STATE();

      if (hm.find(n->hash()) != hm.end()) {
        printf("************* already visited state **************\n");
        // delete n;
      }
      else {
        st.push(n, depth);
        hm.insert(n->hash());
        ++depth;
        i++;

        graphVis->printGraphViz(n.get(), depth);
      }

      printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
    }
    else {
      printf("************* end state **************\n");
      // delete current;
      st.pop();
      --depth;
    }
  }
  printf("number of states : %lu\n", i);
  delete graphVis;
}

void createStateSpaceDFS_RR(const fsm * automata, const TVL * tvl)
{
  elementStack st;
  reachabilityRelation R;
  R.tvl = tvl;
  std::shared_ptr<state> init(initState::createInitState(automata, tvl));
  graphVis = new stateToGraphViz(automata);

  int depth = 0;

  R.getStatus(init.get());
  R.init(init.get());

  st.push(init, depth);

  graphVis->printGraphViz(init.get(), depth);

  unsigned long i = 0;

  while (!st.empty()) {
    auto current = st.top();
    // printf("****************** current state ****************\n");
    // current->s->PRINT_STATE();

    if (!current->init) {
      std::list<std::shared_ptr<state>> sPost_;
      for (auto & p : current->current_state->Post()) {
        std::shared_ptr<state> postState(p);
        sPost_.push_back(postState);
      }
      current->Post = sPost_;
      assert(current->Post.size() > 0);
      current->init = true;
    }

    if (current->Post.size() > 0) {

      auto n = *current->Post.begin();
      current->Post.pop_front();

      // printf("************* pick next state **************\n");

      // n->PRINT_STATE();

      auto status = R.getStatus(n.get());
      R.update(n.get());

      graphVis->printGraphViz(n.get(), depth);

      if (status == STATES_SAME_S1_VISITED) {
        // printf("************* already visited state **************\n");
        // graphVis->printGraphViz(n.get(), depth);
        // delete n;
      }
      // TODO: Sami look at this
      else if (STATES_S1_NEVER_VISITED || STATES_SAME_S1_FRESH) {
        st.push(n, depth);
        // graphVis->printGraphViz(n.get(), depth);
        ++depth;
        i++;
        // graphVis->printGraphViz(n.get(), depth);
      }
      else {
        assert(false);
      }
      // printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
    }
    else {
      // printf("************* end state **************\n");
      // delete current;
      st.pop();
      --depth;
    }
  }

  printf("number of states : %lu\n", i);

  delete graphVis;
}

std::stack<std::shared_ptr<elementStack::element>> reverse(const std::stack<std::shared_ptr<elementStack::element>> & stack)
{
  std::stack<std::shared_ptr<elementStack::element>> reversed;
  auto copy = stack;
  while (!copy.empty()) {
    reversed.push(copy.top());
    copy.pop();
  }
  return reversed;
}

void printElementStack(stateToGraphViz * stateGraphVis, const std::stack<std::shared_ptr<elementStack::element>> & outerStack,
                       const std::stack<std::shared_ptr<elementStack::element>> & innerStack, const state * loopBegin)
{
  state * s = nullptr;
  unsigned int depth = 0;
  stateGraphVis->setIn(stateToGraphViz::PREFIX);
  auto reverseStack = reverse(outerStack);
  std::cout << "\n - Stack trace:\n";
  bool inCycle = false;
  while (!reverseStack.empty()) {
    s = reverseStack.top()->current_state.get();
    depth = reverseStack.top()->depth;
    reverseStack.pop();
    if (loopBegin && loopBegin->hash() == s->hash()) {
      std::cout << "    -- Loop beings here --\n    --\n";
      s->print();
      stateGraphVis->setIn(stateToGraphViz::CYCLE_BEGIN);
      stateGraphVis->printGraphViz(s, depth);
      stateGraphVis->setIn(stateToGraphViz::CYCLE);
      std::cout << "    -- Loop begin repeated in full:\n";
      inCycle = true;
      continue;
    }

    if (inCycle) {
      s->print();
      stateGraphVis->printGraphViz(s, depth);
    }

    // s->print();
    // graphVis->printGraphViz(s, depth);
  }

  if (!loopBegin) {
    std::cout << "    -- Final state repeated in full:\n";
    s->print();
    // graphVis->printGraphViz(s, depth);
  }

  reverseStack = reverse(innerStack);
  while (!reverseStack.empty()) {
    auto top = reverseStack.top();
    auto current_element = top->current_state;
    current_element->print();
    stateGraphVis->printGraphViz(current_element.get(), top->depth);
    reverseStack.pop();
  }
  std::cout << "\n\n ****\n";
}