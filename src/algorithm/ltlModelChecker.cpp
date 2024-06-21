#include "ltlModelChecker.hpp"
#include "explore.hpp"
#include "initState.hpp"
#include "promela_loader.hpp"

bool ltlModelChecker::isNeverClaimProblematic(std::shared_ptr<state> init)
{
  auto neverClaim = init->getNeverClaim();
  if (!neverClaim) {
    std::cerr << "No never claim found. Please check the model before running the model checker." << std::endl;
    return true;
    // throw std::runtime_error("No never claim found. Please check the model before running the model checker.");
  }
  auto neverTrans = neverClaim->transitions();
  if (neverClaim->nullstate() || neverTrans.size() == 0) {
    std::cerr << "Problem with never claim. Please check the model before running the model checker" << std::endl;
    return true;
    // throw std::runtime_error("Problem with never claim. Please check the model before running the model checker");
  }
  transition::erase(neverTrans);
  return false;
}

void ltlModelChecker::printSearchData() const
{
  std::cout << " [explored " << nbStatesExplored << " states, re-explored " << nbStatesReExplored << ", stops " << nbStatesStops
            << "]." << std::endl;
  if (nbStatesExploredInner != 0)
    std::cout << "The inner search explored " << nbStatesExploredInner << " states and re-explored " << nbStatesReExploredInner
              << "." << std::endl;
}


bool ltlModelChecker::check(const std::string & original_file, const TVL * tvl, bool logFiles){
  auto loader = new promela_loader(original_file, tvl);
  auto automata = loader->getAutomata();
  delete loader;
  return check(automata.get(), tvl, logFiles);
}


bool ltlModelChecker::check(const fsm * automata, const TVL * tvl, bool printSearchResults)
{
  // We need to reset the counters before each run - otherwise the counters can be incremented from previous runs
  resetCounters();

  // Create the graphviz object
  graphVis = new stateToGraphViz(automata);

  // Create initial state
  std::shared_ptr<state> init(initState::createInitState(automata, tvl));

  // bool isProblematic = isNeverClaimProblematic(init);

  elementStack stack;
  stack.push(init);

  auto seach_result = outerDFS(stack);
  if (seach_result == 0) {
    printf("Property satisfied");
    printSearchData();
  }
  else {
    /*auto _failProducts = reachableStates.getFailedProducts();
    auto _nbErrors = reachableStates.nbErrors;
    auto _allProductsFail = (tvl->getFeatureModelClauses() & ~_failProducts).IsZero();*/
    printf("\n");
    printf("Non Exhaustive search finished ");
    printSearchData();
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

  if (graphVis != nullptr) {
    delete graphVis;
  }

  return seach_result == 0;
}

bool ltlModelChecker::outerDFS(elementStack & stackOuter, bool printSearchResults, long unsigned maxDepth)
{
  bool exhaustive = false;
  auto firstState = stackOuter.top()->current_state;
  reachableStates.getStatus(firstState.get());
  reachableStates.init(firstState.get());
  // Execution continues as long as the
  //  - stack is not empty
  //  - no error was found (except in the exhaustive case)
  while (somethingToExplore(stackOuter) && !errorFound(reachableStates, exhaustive) && depth < maxDepth) {
    auto currentStateElement = stackOuter.top();
    firstState = currentStateElement->current_state;
    // Check for deadlock
    if (hasError(firstState, state::ERR_DEADLOCK)) {
      // Deadlock found
      if (printSearchResults)
        printElementStack(graphVis, stackOuter.stackElem);
    }
    if (propertyViolated(firstState)) {
      std::cout << "The LTL property is violated." << std::endl;
      if (printSearchResults) {
        std::cout << "The trace is: " << std::endl;
        printElementStack(graphVis, stackOuter.stackElem);
      }
      reachableStates.addTraceViolation(firstState.get());
    }

    if (firstState->safetyPropertyViolation()) {
      // Safety property violated.
      // We have to pop two states off the stack to get to the violating state:
      //  -> the currentStateElement top is a skip transition fired from an accepting state
      //  -> the state below that is the accepting state
      //  -> the state below that is the state that actually led to the accepting state to be reachable.
      //     i.e. this state is the actual violating state.

      printf("Safety property violated %lu.\n", firstState->hash());
      if (printSearchResults)
        printElementStack(graphVis, stackOuter.stackElem);

      // Why don't we break here?
      reachableStates.addTraceViolation(firstState.get());
      stackOuter.pop(3);
      depth -= 3;
    }
    else {
      // Otherwise, the state can be explored (or exploration continue)
      // currentStateElement->setErrorStatus = _nbErrors;
      // ..., or there is a transition to be executed:
      if (currentStateElement->hasSuccessors()) {
        // There are still successors to explore
        auto firstSuccessor = currentStateElement->nextSuccessor();
        // graphVis->printGraphViz(s_);

        if (assertionViolated(firstSuccessor)) {
          // printf("Assertion at line %d violated", *s_->getOrigin()->lines.begin());
          reachableStates.addTraceViolation(currentStateElement->current_state.get());
          // delete s_;
          firstSuccessor = nullptr;
        }
        else {
          // get the status before update!
          auto status = reachableStates.getStatus(firstSuccessor.get());
          // graphVis->printGraphViz(s_, depth);
          if (status == STATES_SAME_S1_VISITED) {
            // The stat has been visited before
            // s_->print();
            // delete s_;

            nbStatesStops++;
          }
          else {
            switch (status) {
            // case STATES_SAME_S1_VISITED:
            //   // The state is not a new state - it has been visited before
            //   nbStatesStops++;
            //   break;
            case STATES_SAME_S1_FRESH:
              // The state is not a new state - it has been visited before
              nbStatesReExplored++;
              break;
            case STATES_S1_NEVER_VISITED:
              // The state is not a new state - it has been visited before
              nbStatesExplored++;
              break;
            default:
              break;
            }

            reachableStates.update(firstSuccessor.get());
            // assert(reachableStates.getStatus(s_.get()) != status);
            depth++;
            stackOuter.push(firstSuccessor, depth);
          }
        }
      }
      else {
        auto currentState = currentStateElement->current_state;
        // Back these values up, the inner search will free currentStateElement->state before returning
        if (currentState->isAccepting()) {
          // Accepting state found - start inner search to find accepting cycle
          depth++;
          nbStatesExploredInner++;
          // printf("    +-> found accepting state %lu, starting inner...\n", s_hash);
          elementStack stackInner;
          std::shared_ptr<state> s_ptr(currentState->deepCopy());
          stackInner.push(s_ptr, depth);

          // error needs to be to the right, for otherwise lazy evaluation might cause the innerDFS call to be skipped
          reachableStates.setDFS(reachabilityRelation::DFS_INNER);
          innerDFS(stackInner, stackOuter);
          reachableStates.setDFS(reachabilityRelation::DFS_OUTER);
          // it will have been destroyed when the innerDFS backtracked for the last time
        }

        // currentStateElement->s = nullptr;
        stackOuter.pop();
        depth--;
        // printf("    +-> State %lu erase from the hast table.\n", s_hash);
      }
    } // explore state
  }   // end while

  assert(stackOuter.empty() || !exhaustive);
  emptyStack(stackOuter);
  // TVL::printBool(reachableStates.getFailedProducts());
  return reachableStates.hasErrors();
}

bool ltlModelChecker::innerDFS(elementStack & stackInner, const elementStack & stackOuter, bool printSearchResults,
                               long unsigned maxDepth)
{
  bool exhaustive = false;
  // Execution continues as long as the
  //  - stack is not empty
  //  - no error was found (except in the exhaustive case)
  while (somethingToExplore(stackInner) && !errorFound(reachableStates, exhaustive) && depth < maxDepth) {
    auto currentStateElement = stackInner.top();
    // Check for deadlock
    checkForDeadlock(currentStateElement->current_state, stackOuter, printSearchResults);

    // If we have explored all transitions of the state (!currentStateElement->E_never; see "struct stackElt"
    // in stack.h), we check whether the state is accepting and start a backlink search if it is;
    // otherwise just backtrack
    if (currentStateElement->hasSuccessors()) {
      // There are still successors to explore
      auto firstSuccessor = currentStateElement->nextSuccessor();

      bool onStack = stackOuter.isIn(firstSuccessor->hash());

      if (onStack || assertionViolated(firstSuccessor)) {
        // Error found
        if (onStack) {
          std::cerr << "Error: Property violated" << std::endl;
        }
        stackInner.push(firstSuccessor, depth + 1);
        if (printSearchResults)
          printElementStack(graphVis, stackOuter.stackElem, stackInner.stackElem, firstSuccessor.get());
        stackInner.pop();

        reachableStates.addTraceViolation(currentStateElement->current_state.get());
      }
      else {
        // get the status before update!
        auto status = reachableStates.getStatus(firstSuccessor.get());
        auto lastFoundIn = reachableStates.lastFoundIn(firstSuccessor.get());
        // update put to inner if outer
        reachableStates.update(firstSuccessor.get());
        assert(reachableStates.lastFoundIn(firstSuccessor.get()) == reachabilityRelation::DFS_INNER);

        switch (status) {
        case STATES_SAME_S1_VISITED:
          // The state is not a new state:
          nbStatesStops++;
          break;
        case STATES_SAME_S1_FRESH:
          // The state is not a new state:
          if (lastFoundIn == reachabilityRelation::DFS_INNER) {
            nbStatesReExploredInner++;
          }
          else {
            nbStatesExploredInner++;
          }
          depth++;
          nbStatesReExplored++;
          stackInner.push(firstSuccessor, depth);
          break;
        default:
          printElementStack(graphVis, stackOuter.stackElem, stackInner.stackElem, firstSuccessor.get());
          throw std::runtime_error("Bug! The above state was found during the inner DFS but not during the outer! Aborting.");
          break;
        }
        // fresh state
        // no assert violation
      }
    }
    else {
      // No more successors to explore => backtracking
      stackInner.pop();
      depth--;
    }

  } // end while

  emptyStack(stackInner);
  return reachableStates.hasErrors();
}

/// @brief A function that checks if a formula is satisfied by an automata within a given bound
/// @param automata a pointer to the automata
/// @param tvl a pointer to the tvl
/// @param bound the number of steps to maximally take
/// @return true if the formula is satisfied by the automata within the given bound, false otherwise
// bool ltlModelChecker::check(const fsm * automata, const TVL * tvl, unsigned int bound) {}

void ltlModelChecker::emptyStack(elementStack & stack)
{
  while (!stack.empty()) {
    stack.pop();
  }
}

void ltlModelChecker::resetCounters()
{
  reachableStates = reachabilityRelation();
  nbStatesReExplored = 0;
  nbStatesExplored = 0;
  nbStatesStops = 0;
  nbStatesExploredInner = 0;
  nbStatesReExploredInner = 0;
  depth = 0;
}

bool ltlModelChecker::hasError(const std::shared_ptr<state> s, const unsigned int errorMask, const elementStack & stack,
                               bool printStack) const
{
  auto hasError = (errorMask & s->getErrorMask());
  if (hasError) {
    if (printStack)
      printElementStack(graphVis, stack.stackElem);
  }
  return hasError;
}

void ltlModelChecker::checkForDeadlock(const std::shared_ptr<state> s, const elementStack & stack, bool printStack)
{
  auto deadlockOccurred = hasError(s, state::ERR_DEADLOCK, stack, printStack);
  if (deadlockOccurred) {
    throw std::runtime_error("Deadlock found");
  }
}

bool ltlModelChecker::propertyViolated(const std::shared_ptr<state> s) const
{
  auto violated = hasError(s, state::ERR_PROPERTY_VIOLATION);
  if (violated) {
    printf("Property violated\n");
    s->print();
  }
  return violated;
}

bool ltlModelChecker::assertionViolated(const std::shared_ptr<state> s) const
{
  auto violated = hasError(s, state::ERR_ASSERT_FAIL);
  if (violated) {
    printf("Assertion violated\n");
  }
  return violated;
}

bool ltlModelChecker::somethingToExplore(const elementStack & stack) { return !stack.empty(); }

bool ltlModelChecker::errorFound(const reachabilityRelation & reachableStates, bool exhaustive)
{
  return reachableStates.hasErrors() && !exhaustive;
}
