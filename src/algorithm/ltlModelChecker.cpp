#include "ltlModelChecker.hpp"
#include "explore.hpp"
#include "initState.hpp"

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

bool ltlModelChecker::check(const fsm * automata, const TVL * tvl, bool generateIntermediaryFiles)
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
    printf(" [explored %lu states, re-explored %lu, stops %lu].\n", nbStatesExplored, nbStatesReExplored, nbStatesStops);
    if (nbStatesExploredInner != 0)
      printf("The inner search explored %lu states and re-explored %lu.\n", nbStatesExploredInner, nbStatesReExploredInner);
  }
  else {
    /*auto _failProducts = reachableStates.getFailedProducts();
    auto _nbErrors = reachableStates.nbErrors;
    auto _allProductsFail = (tvl->getFeatureModelClauses() & ~_failProducts).IsZero();*/
    printf("\n");
    printf("Non Exhaustive search finished ");
    printf(" [explored %lu states, re-explored %lu].\n", nbStatesExplored, nbStatesReExplored);
    if (nbStatesExploredInner != 0)
      printf("The inner search explored %lu states and re-explored %lu.\n", nbStatesExploredInner, nbStatesReExploredInner);

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

bool ltlModelChecker::somethingToExplore(const elementStack & stack) { return !stack.empty(); }

bool ltlModelChecker::errorFound(const reachabilityRelation & reachableStates, bool exhaustive)
{
  return reachableStates.hasErrors() && !exhaustive;
}

byte ltlModelChecker::outerDFS(elementStack & stackOuter, bool generateIntermediaryFiles, long unsigned maxDepth)
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
    checkForDeadlock(currentStateElement->current_state, stackOuter, generateIntermediaryFiles);

    if (firstState->safetyPropertyViolation()) {
      // Safety property violated.
      // We have to pop two states off the stack to get to the violating state:
      //  -> the currentStateElement top is a skip transition fired from an accepting state
      //  -> the state below that is the accepting state
      //  -> the state below that is the state that actually led to the accepting state to be reachable.
      //     i.e. this state is the actual violating state.

      printf("Safety property violated %lu.\n", firstState->hash());
      if (generateIntermediaryFiles)
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
      if (!currentStateElement->Post.empty()) {
        // There are still successors to explore
        auto firstSuccessor = *currentStateElement->Post.begin();
        currentStateElement->Post.pop_front();

        // graphVis->printGraphViz(s_);

        if (firstSuccessor->getErrorMask() & state::ERR_ASSERT_FAIL) {
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
        // printf("    +-> all transitions of state %lu fired, acceptance check and backtracking...\n", s_hash);
        // Back these values up, the inner search will free currentStateElement->state before returning
        if (currentState->isAccepting()) {
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

void ltlModelChecker::checkForDeadlock(const std::shared_ptr<state> s, const elementStack & stack, bool printStack)
{
  auto errorMask = s->getErrorMask();
  if (errorMask & state::ERR_DEADLOCK) {
    if (printStack)
      printElementStack(graphVis, stack.stackElem);
    throw std::runtime_error("Deadlock found");
  }
  else if (errorMask & state::ERR_PROPERTY_VIOLATION) {
    std::cout << "Property violated" << std::endl;
    if (printStack)
      s->print();
  }
}

byte ltlModelChecker::innerDFS(elementStack & stackInner, const elementStack & stackOuter, bool generateIntermediaryFiles,
                               long unsigned maxDepth)
{
  bool exhaustive = false;
  // Execution continues as long as the
  //  - stack is not empty
  //  - no error was found (except in the exhaustive case)
  while (somethingToExplore(stackInner) && !errorFound(reachableStates, exhaustive) && depth < maxDepth) {
    auto currentStateElement = stackInner.top();
    // Check for deadlock
    checkForDeadlock(currentStateElement->current_state, stackOuter, generateIntermediaryFiles);

    // If we have explored all transitions of the state (!currentStateElement->E_never; see "struct stackElt"
    // in stack.h), we check whether the state is accepting and start a backlink search if it is;
    // otherwise just backtrack
    if (currentStateElement->Post.empty()) {
      // No more successors to explore => backtracking
      stackInner.pop();
      depth--;
    }
    else {
      // There are still successors to explore
      auto firstSuccessor = *currentStateElement->Post.begin();
      currentStateElement->Post.pop_front();

      bool onStack = stackOuter.isIn(firstSuccessor->hash());

      // graphVis->printGraphViz(s_);

      if (onStack || firstSuccessor->getErrorMask() & state::ERR_ASSERT_FAIL) {
        // Error found
        if (onStack) {
          std::cerr << "Error: Property violated" << std::endl;
        }
        stackInner.push(firstSuccessor, depth + 1);
        if (generateIntermediaryFiles)
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

        if (status == STATES_SAME_S1_VISITED) {
          // The state is not a new state:
          nbStatesStops++;
        }
        else if (status == STATES_SAME_S1_FRESH) {
          // The state is not a new state:
          if (lastFoundIn == reachabilityRelation::DFS_INNER) {
            // printf("                 - inner state %lu visited, but features fresh\n", s_hash);
            nbStatesReExploredInner++;
          }
          else {
            // printf("                 - inner state %lu only visited during outer search\n", s_hash);
            nbStatesExploredInner++;
          }
          // done by the reachability relation object logic
          // graphVis->printGraphViz(s_);
          depth++;
          nbStatesReExplored++;
          // will put to inner if it was outer
          stackInner.push(firstSuccessor, depth);
        }
        else {
          printElementStack(graphVis, stackOuter.stackElem, stackInner.stackElem, firstSuccessor.get());
          throw std::runtime_error("Bug! The above state was found during the inner DFS but not during the outer! Aborting.");
        }
        // fresh state
        // no assert violation
      }
    } // fire post
  }   // end while

  // If error is true and we end up here, then we're in exhaustive mode. A summary has to be printed
  // if(error /* not needed: && exhaustive */
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