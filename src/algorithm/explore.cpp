#include "explore.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>

#include "initState.hpp"
#include "process.hpp"
#include "semantic.hpp"
#include "stateToGraphViz.hpp"
#include "transition.hpp"

#define PRINT_STATE print

#define B 5

stateToGraphViz * graphVis = nullptr;

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

transition * most_similar_transition(const std::list<transition *> transitions, const transition * current)
{
  transition * most_similar = nullptr;
  double max_similarity = 0;
  for (auto t : transitions) {
    double similarity = 0; /*t->similarity(current)*/
    if (similarity > max_similarity) {
      max_similarity = similarity;
      most_similar = t;
    }
  }
  return most_similar;
}

state * most_similar_state(const state * current, const std::list<state *> states)
{
  state * most_similar = nullptr;
  // Similarity is measured by the delta function in the range of [0, 1]
  double max_similarity = 1;
  for (auto s : states) {
    double delta = s->delta(current);
    if (delta < max_similarity) {
      max_similarity = delta;
      most_similar = s;
    }
  }
  return most_similar;
}

std::list<state *> distinct_states(const std::list<state *> & states_original, const std::list<state *> & states_mutant)
{
  std::list<state *> distinct;
  for (auto & s : states_original) {
    bool found = false;
    for (auto & d : states_mutant) {
      if (s->isSame(d)) {
        found = true;
        break;
      }
    }
    if (!found) {
      distinct.push_back(s);
    }
  }
  return distinct;
}

std::unique_ptr<trace> interactiveDebugging(const std::shared_ptr<fsm> automata, const size_t trace_length, const TVL * tvl)
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
    for (auto & p : post_states) {
      std::cout << index << " : ";
      p->printDelta(current_state);
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

  std::string myline;

  if (myfile.is_open()) {
    while (myfile) { // equivalent to myfile.good()
      std::getline(myfile, myline);
      scheduler.push_back(myline);
      std::cout << myline << '\n';
    }
  }
  else {
    std::cerr << "Couldn't open file\n";
  }

  transition * trans = nullptr;

  do {

    auto sVariable = current->getVariable("s");
    int sValue = dynamic_cast<primitiveVariable *>(sVariable)->getValue();
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
    i++;
    printf("****************** current state ****************\n");
    current->s->PRINT_STATE();
    st.pop();
    depth = current->depth;

    auto successors = current->s->Post();
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

  printf("number of states : %ld\n", i);

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

    printf("****************** current state ****************\n");
    current->s->PRINT_STATE();

    if (!current->init) {
      std::list<std::shared_ptr<state>> sPost_;
      for (auto & p : current->s->Post()) {
        std::shared_ptr<state> postState(p);
        sPost_.push_back(postState);
      }
      current->Post = sPost_;
      current->init = true;
    }

    if (current->Post.size() > 0) {

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

  printf("number of states : %ld\n", i);
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
      for (auto & p : current->s->Post()) {
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

  printf("number of states : %ld\n", i);

  delete graphVis;
}

// static 		unsigned int _nbErrors = 0;				// Total number of encountered problems
static long unsigned int _nbStatesExplored = 1; // Total of distinct states (without features) explored
static long unsigned int _nbStatesReExplored =
    0; // Total of states that had to be re-explored because new products were found to be able to reach them
static long unsigned int _nbStatesStops =
    0; // Total of states where exploration backtracked because they were already visited before
static long unsigned int _nbStatesExploredInner = 0;   // As before, but for inner search.
static long unsigned int _nbStatesReExploredInner = 0; // As before, but for inner search.
static long unsigned int _nbStatesStopsInner = 0;      // As before, but for inner search.
static long unsigned int _depth = 0;                   // Current exploration depth (inner and outer)

bool ltlModelChecker::check(const fsm * automata, const TVL * tvl)
{
  R = reachabilityRelation();

  _nbStatesExplored = 0;
  _nbStatesReExplored = 0;

  // Create initial state
  std::shared_ptr<state> init(initState::createInitState(automata, tvl));

  auto neverClaim = init->getNeverClaim();
  assert(neverClaim);
  auto neverTrans = neverClaim->transitions();
  if (!neverClaim || neverClaim->nullstate() || neverTrans.size() == 0) {
    printf("init->never is NULL\n");
    assert(false);
  }

  transition::erase(neverTrans);

  elementStack stack;
  stack.push(init);

  printf("state size : %lu\n", init->getSizeOf());

  auto seach_result = outerDFS(stack);
  return seach_result == 0;
}

// Can use the check method to check if the property is satisfied
void ltlModelChecker::startNestedDFS(const fsm * automata, const TVL * tvl)
{
  _nbStatesExplored = 0;
  _nbStatesReExplored = 0;
  printf("[startNestedDFS]\n");

  // Create initial state
  std::shared_ptr<state> init(initState::createInitState(automata, tvl));

  graphVis = new stateToGraphViz(automata);

  // graphVis->printGraphViz(init, _depth);

  // init->print();

  // Sanity checks
  auto neverClaim = init->getNeverClaim();
  assert(neverClaim);
  auto neverTrans = neverClaim->transitions();
  if (!neverClaim || neverClaim->nullstate() || neverTrans.size() == 0) {
    printf("init->never is NULL\n");
    assert(false);
  }

  transition::erase(neverTrans);

  elementStack stack;
  stack.push(init);

  printf("state size : %lu\n", init->getSizeOf());

  if (outerDFS(stack) == 0) {
    printf("Property satisfied");
    printf(" [explored %lu states, re-explored %lu, stops %lu].\n", _nbStatesExplored, _nbStatesReExplored, _nbStatesStops);
    if (_nbStatesExploredInner != 0)
      printf("The inner search explored %lu states and re-explored %lu.\n", _nbStatesExploredInner, _nbStatesReExploredInner);
  }
  else {
    /*auto _failProducts = R.getFailedProducts();
    auto _nbErrors = R.nbErrors;
    auto _allProductsFail = (tvl->getFeatureModelClauses() & ~_failProducts).IsZero();*/
    printf("\n");
    printf("Non Exhaustive search finished ");
    printf(" [explored %lu states, re-explored %lu].\n", _nbStatesExplored, _nbStatesReExplored);
    if (_nbStatesExploredInner != 0)
      printf("The inner search explored %lu states and re-explored %lu.\n", _nbStatesExploredInner, _nbStatesReExploredInner);

    // if(_nbErrors == 1) printf(" -  One problem found");
    // else printf(" - %u problems were found", _nbErrors);
    /*if(_allProductsFail || isTautology(_failProducts))
            printf(" covering every product.\n");
    else {
            printf(" covering the following products (others are ok):\n");
            TVL::printBool(_failProducts);
            printf("\n");
    }*/
    printf("\n");
  }

  delete graphVis;
}

int i = 0;

byte ltlModelChecker::outerDFS(elementStack & stackOuter)
{

  byte exhaustive = 0;

  R.getStatus(stackOuter.top()->s.get());
  R.init(stackOuter.top()->s.get());

  // graphVis->printGraphViz(stackOuter.top()->s);

  // Execution continues as long as the

  //  - stack is not empty
  //  - no error was found (except in the exhaustive case)
  while (!stackOuter.empty() && (!R.hasErrors() || exhaustive) /*&& !R.isComplete()*/) {

    auto current = stackOuter.top();
    // graphVis->printGraphViz(current->s);

    auto s_hash = current->s->hash();

    if (current->s->getErrorMask() & state::ERR_DEADLOCK) {

      printf("Found deadlock");
      printElementStack(stackOuter.stackElem);
      assert(false);
    }
    else if (current->s->safetyPropertyViolation()) {
      // Safety property violated.
      // We have to pop two states off the stack to get to the violating state:
      //  -> the current top is a skip transition fired from an accepting state
      //  -> the state below that is the accepting state
      //  -> the state below that is the state that actually led to the accepting state to be reachable.
      //     i.e. this state is the actual violating state.

      printf("Safety property violated %lu.\n", s_hash);
      printElementStack(stackOuter.stackElem);

      R.addTraceViolation(current->s.get());

      stackOuter.pop();

      // auto newTop = stackOuter.top();
      // graphVis->printGraphViz(newTop->s);

      stackOuter.pop();

      // newTop = stackOuter.top();
      // graphVis->printGraphViz(newTop->s);

      stackOuter.pop();

      _depth -= 3;

      // Otherwise, the state can be explored (or exploration continue)
    }
    else {

      // current->s->print();
      // printf("    +-> exploring %lu...\n", s_hash);
      // current->setErrorStatus = _nbErrors;

      // ..., or there is a transition to be executed:
      if (current->Post.size() > 0) {

        // printf("    +-> peecking state %lu...\n", s_hash);

        auto s_ = *current->Post.begin();
        s_hash = s_->hash();

        current->Post.pop_front();

        // s_->print();

        // graphVis->printGraphViz(s_);

        // s_->print();

        if (s_->getErrorMask() & state::ERR_ASSERT_FAIL) {
          // printf("Assertion at line %d violated", *s_->getOrigin()->lines.begin());

          R.addTraceViolation(current->s.get());

          // delete s_;
          s_ = nullptr;
        }
        else {

          // get the status before update!
          auto status = R.getStatus(s_.get());
          // graphVis->printGraphViz(s_, _depth);

          if (status == STATES_SAME_S1_VISITED) {
            // printf("         - state %lu already visited.\n", s_hash);
            // s_->print();
            // delete s_;

            _nbStatesStops++;
          }
          else {

            // graphVis->printGraphViz(s_, _depth);

            if (status == STATES_SAME_S1_FRESH) {

              // The state is not a new state:
              // printf("         - state %lu visited but features fresh, pushing on stack.\n", s_hash);

              // graphVis->printGraphViz(s_);

              _nbStatesReExplored++;
            }
            else if (status == STATES_S1_NEVER_VISITED) {

              // graphVis->printGraphViz(s_, _depth);

              _nbStatesExplored++;

              // printf("         - state fresh %lu, pushing on stack.\n", s_hash);
            }

            R.update(s_.get());
            // assert(R.getStatus(s_.get()) != status);

            _depth++;
            stackOuter.push(s_, _depth);
          }
        }
      }
      else if (current->Post.size() == 0) {

        s_hash = current->s->hash();

        // printf("    +-> all transitions of state %lu fired, acceptance check and backtracking...\n", s_hash);

        // Back these values up, the inner search will free current->state before returning

        if (current->s->isAccepting()) {
          _depth++;
          _nbStatesExploredInner++;

          // printf("    +-> found accepting state %lu, starting inner...\n", s_hash);
          elementStack stackInner;
          std::shared_ptr<state> s_ptr(current->s->deepCopy());
          stackInner.push(s_ptr, _depth);

          // error needs to be to the right, for otherwise lazy evaluation might cause the innerDFS call to be skipped
          R.setDFS(reachabilityRelation::DFS_INNER);
          innerDFS(stackInner, stackOuter);
          R.setDFS(reachabilityRelation::DFS_OUTER);
          // it will have been destroyed when the innerDFS backtracked for the last time
          // delete current->s;
        }

        // current->s = nullptr;
        stackOuter.pop();
        _depth--;
        // printf("    +-> State %lu erase from the hast table.\n", s_hash);
      }

    } // explore state
  }   // end while

  // If error is true and we end up here, then we're in exhaustive mode. A summary has to be printed
  // if(error /* not needed: && exhaustive */

  assert(stackOuter.empty() || !exhaustive);
  while (!stackOuter.empty()) {
    stackOuter.pop();
  }

  // TVL::printBool(R.getFailedProducts());
  return R.hasErrors();
}

byte ltlModelChecker::innerDFS(elementStack & stackInner, const elementStack & stackOuter)
{

  byte exhaustive = 0;

  // Execution continues as long as the
  //  - stack is not empty
  //  - no error was found (except in the exhaustive case)
  while (!stackInner.empty() && (!R.hasErrors() || exhaustive) /*&& !R.isComplete()*/) {

    auto current = stackInner.top();
    auto s_hash = current->s->hash();

    // current->s->print();
    // printf("    +-> inner exploring %lu...\n", s_hash);
    // current->setErrorStatus = _nbErrors;

    if (current->s->getErrorMask() & state::ERR_DEADLOCK) {

      printf("Found deadlock");
      printElementStack(stackOuter.stackElem);
      assert(false);
    }

    // If we have explored all transitions of the state (!current->E_never; see "struct stackElt"
    // in stack.h), we check whether the state is accepting and start a backlink search if it is;
    // otherwise just backtrack
    if (current->Post.size() == 0) {
      // printf("    +-> inner all transitions of state %lu fired, backtracking...\n", s_hash);
      stackInner.pop();
      _depth--;

      // ..., or there is a transition to be executed:
    }
    else if (current->Post.size() > 0) {

      // printf("    +-> inner peecking state %lu...\n", s_hash);

      auto s_ = *current->Post.begin();
      current->Post.pop_front();

      // s_->print();

      s_hash = s_->hash();
      bool onSt = stackOuter.isIn(s_hash);

      // graphVis->printGraphViz(s_);

      if (onSt || s_->getErrorMask() & state::ERR_ASSERT_FAIL) {

        if (onSt) {
          printf("Property violated\n");
        }
        else {
          // printf("Assertion at line %d violated", *s_->getOrigin()->lines.begin());
        }

        stackInner.push(s_, _depth + 1);
        printElementStack(stackOuter.stackElem, stackInner.stackElem, s_.get());
        stackInner.pop();

        R.addTraceViolation(current->s.get());
      }
      else {

        // get the status before update!
        auto status = R.getStatus(s_.get());

        // graphVis->printGraphViz(s_, _depth);

        auto lastFoundIn = R.lastFoundIn(s_.get());
        // update put to inner if outer
        R.update(s_.get());
        assert(R.lastFoundIn(s_.get()) == reachabilityRelation::DFS_INNER);

        if (status == STATES_SAME_S1_VISITED) {
          // printf("         - inner state %lu already visited.\n", s_hash);
          _nbStatesStops++;
        }
        else if (status == STATES_SAME_S1_FRESH) {

          // The state is not a new state:
          if (lastFoundIn == reachabilityRelation::DFS_INNER) {
            // printf("                 - inner state %lu visited, but features fresh\n", s_hash);
            _nbStatesReExploredInner++;
          }
          else {
            // printf("                 - inner state %lu only visited during outer search\n", s_hash);
            _nbStatesExploredInner++;
          }
          // done by the reachability relation object logic
          // graphVis->printGraphViz(s_);

          _depth++;
          _nbStatesReExplored++;

          // will put to inner if it was outer
          stackInner.push(s_, _depth);
        }
        else {

          printElementStack(stackOuter.stackElem, stackInner.stackElem, s_.get());
          printf("Bug! The above state was found during the inner DFS but not during the outer! Aborting.\n");
          assert(false);
        }
        // fresh state

        // no assert violation
      }
    } // fire post

  } // end while

  // If error is true and we end up here, then we're in exhaustive mode. A summary has to be printed
  // if(error /* not needed: && exhaustive */

  while (!stackInner.empty())
    stackInner.pop();

  return R.hasErrors();
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

void printElementStack(const std::stack<std::shared_ptr<elementStack::element>> & outerStack,
                       const std::stack<std::shared_ptr<elementStack::element>> & innerStack, const state * loopBegin)
{
  state * s = nullptr;
  unsigned int depth = 0;
  graphVis->setIn(stateToGraphViz::PREFIX);
  auto reverseStack = reverse(outerStack);
  std::cout << "\n - Stack trace:\n";
  bool inCycle = false;
  while (!reverseStack.empty()) {
    s = reverseStack.top()->s.get();
    depth = reverseStack.top()->depth;
    reverseStack.pop();
    if (loopBegin && loopBegin->hash() == s->hash()) {
      std::cout << "    -- Loop beings here --\n    --\n";
      s->print();
      graphVis->setIn(stateToGraphViz::CYCLE_BEGIN);
      graphVis->printGraphViz(s, depth);
      graphVis->setIn(stateToGraphViz::CYCLE);
      std::cout << "    -- Loop begin repeated in full:\n";
      inCycle = true;
      continue;
    }

    if (inCycle) {
      s->print();
      graphVis->printGraphViz(s, depth);
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
    top->s->print();
    graphVis->printGraphViz(top->s.get(), top->depth);
    reverseStack.pop();
  }
  std::cout << "\n\n ****\n";
}