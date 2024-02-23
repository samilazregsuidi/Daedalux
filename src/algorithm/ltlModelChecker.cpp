#include "ltlModelChecker.hpp"
#include "explore.hpp"
#include "initState.hpp"

void ltlModelChecker::checkNeverClaim(std::shared_ptr<state> init)
{
  auto neverClaim = init->getNeverClaim();
  assert(neverClaim);
  auto neverTrans = neverClaim->transitions();
  if (!neverClaim || neverClaim->nullstate() || neverTrans.size() == 0) {
    printf("Problem with never claim. Please check the model, before running the model checker.\n");
    assert(false);
  }
  transition::erase(neverTrans);
}

bool ltlModelChecker::check(const fsm * automata, const TVL * tvl, bool generateIntermediaryFiles)
{
  // We need to reset the counters before each run - otherwise the counters can be incremented from previous runs
  resetCounters();

  // Create the graphviz object
  graphVis = new stateToGraphViz(automata);

  // Create initial state
  std::shared_ptr<state> init(initState::createInitState(automata, tvl));

  checkNeverClaim(init);

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
    /*auto _failProducts = R.getFailedProducts();
    auto _nbErrors = R.nbErrors;
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

byte ltlModelChecker::outerDFS(elementStack & stackOuter, bool generateIntermediaryFiles)
{
  byte exhaustive = 0;

  R.getStatus(stackOuter.top()->s.get());
  R.init(stackOuter.top()->s.get());

  // Execution continues as long as the
  //  - stack is not empty
  //  - no error was found (except in the exhaustive case)
  while (!stackOuter.empty() && (!R.hasErrors() || exhaustive) /*&& !R.isComplete()*/) {
    auto current = stackOuter.top();
    auto s_hash = current->s->hash();

    // Check for deadlock
    if (current->s->getErrorMask() & state::ERR_DEADLOCK) {
      printf("Found deadlock");
      printElementStack(graphVis, stackOuter.stackElem);
      assert(false);
    }

    if (current->s->safetyPropertyViolation()) {
      // Safety property violated.
      // We have to pop two states off the stack to get to the violating state:
      //  -> the current top is a skip transition fired from an accepting state
      //  -> the state below that is the accepting state
      //  -> the state below that is the state that actually led to the accepting state to be reachable.
      //     i.e. this state is the actual violating state.

      printf("Safety property violated %lu.\n", s_hash);
      printElementStack(graphVis, stackOuter.stackElem);

      R.addTraceViolation(current->s.get());

      stackOuter.pop();

      // auto newTop = stackOuter.top();
      // graphVis->printGraphViz(newTop->s);

      stackOuter.pop();

      // newTop = stackOuter.top();
      // graphVis->printGraphViz(newTop->s);

      stackOuter.pop();
      depth -= 3;

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
          // graphVis->printGraphViz(s_, depth);

          if (status == STATES_SAME_S1_VISITED) {
            // printf("         - state %lu already visited.\n", s_hash);
            // s_->print();
            // delete s_;

            nbStatesStops++;
          }
          else {
            // graphVis->printGraphViz(s_, depth);
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

            R.update(s_.get());
            // assert(R.getStatus(s_.get()) != status);
            depth++;
            stackOuter.push(s_, depth);
          }
        }
      }
      else if (current->Post.size() == 0) {
        s_hash = current->s->hash();

        // printf("    +-> all transitions of state %lu fired, acceptance check and backtracking...\n", s_hash);

        // Back these values up, the inner search will free current->state before returning

        if (current->s->isAccepting()) {
          depth++;
          nbStatesExploredInner++;
          // printf("    +-> found accepting state %lu, starting inner...\n", s_hash);
          elementStack stackInner;
          std::shared_ptr<state> s_ptr(current->s->deepCopy());
          stackInner.push(s_ptr, depth);

          // error needs to be to the right, for otherwise lazy evaluation might cause the innerDFS call to be skipped
          R.setDFS(reachabilityRelation::DFS_INNER);
          innerDFS(stackInner, stackOuter);
          R.setDFS(reachabilityRelation::DFS_OUTER);
          // it will have been destroyed when the innerDFS backtracked for the last time
          // delete current->s;
        }

        // current->s = nullptr;
        stackOuter.pop();
        depth--;
        // printf("    +-> State %lu erase from the hast table.\n", s_hash);
      }

    } // explore state
  }   // end while

  assert(stackOuter.empty() || !exhaustive);

  emptyStack(stackOuter);

  // TVL::printBool(R.getFailedProducts());
  return R.hasErrors();
}

void ltlModelChecker::emptyStack(elementStack & stack)
{
  while (!stack.empty()) {
    stack.pop();
  }
}

byte ltlModelChecker::innerDFS(elementStack & stackInner, const elementStack & stackOuter, bool generateIntermediaryFiles)
{
  byte exhaustive = 0;

  // Execution continues as long as the
  //  - stack is not empty
  //  - no error was found (except in the exhaustive case)
  while (!stackInner.empty() && (!R.hasErrors() || exhaustive) /*&& !R.isComplete()*/) {
    auto current = stackInner.top();

    auto s_hash = current->s->hash();

    // Check for deadlock
    if (current->s->getErrorMask() & state::ERR_DEADLOCK) {
      printf("Found deadlock");
      printElementStack(graphVis, stackOuter.stackElem);
      assert(false);
    }

    // If we have explored all transitions of the state (!current->E_never; see "struct stackElt"
    // in stack.h), we check whether the state is accepting and start a backlink search if it is;
    // otherwise just backtrack
    if (current->Post.size() == 0) {
      // printf("    +-> inner all transitions of state %lu fired, backtracking...\n", s_hash);
      stackInner.pop();
      depth--;

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
        stackInner.push(s_, depth + 1);
        printElementStack(graphVis, stackOuter.stackElem, stackInner.stackElem, s_.get());
        stackInner.pop();

        R.addTraceViolation(current->s.get());
      }
      else {

        // get the status before update!
        auto status = R.getStatus(s_.get());

        // graphVis->printGraphViz(s_, depth);

        auto lastFoundIn = R.lastFoundIn(s_.get());
        // update put to inner if outer
        R.update(s_.get());
        assert(R.lastFoundIn(s_.get()) == reachabilityRelation::DFS_INNER);

        if (status == STATES_SAME_S1_VISITED) {
          // printf("         - inner state %lu already visited.\n", s_hash);
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
          stackInner.push(s_, depth);
        }
        else {
          printElementStack(graphVis, stackOuter.stackElem, stackInner.stackElem, s_.get());
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
  emptyStack(stackInner);

  return R.hasErrors();
}

void ltlModelChecker::resetCounters()
{
  R = reachabilityRelation();
  nbStatesReExplored = 0;
  nbStatesExplored = 0;
  nbStatesStops = 0;
  nbStatesExploredInner = 0;
  nbStatesReExploredInner = 0;
  depth = 0;
}