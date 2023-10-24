#include "explore.hpp"

#include <stdio.h>
#include <algorithm>

#include <iostream>
#include <fstream>
#include <string>

#include "state.hpp"
#include "transition.hpp"
#include "process.hpp"

#include "initState.hpp"

//bad coupling?

#include "semantic.hpp"

#include "stateToGraphViz.hpp"

#include "reachabilityRelation.hpp"

#define PRINT_STATE print

#define B 5

stateToGraphViz* graphVis = nullptr;

void launchExecution(const fsm* automata, const TVL* tvl) {
	state* current = initState::createInitState(automata, tvl);
	unsigned long i = 0;
	//printf("**********************************\n");
	current->PRINT_STATE();

	graphVis = new stateToGraphViz(automata);

	graphVis->printGraphViz(current);

	while(transition* trans = transition::sampleUniform(current->executables())){
		++i;
		current->apply(trans);
		//printf("--------------------------------------\n");
		current->PRINT_STATE();
		graphVis->printGraphViz(current);

		if(current->isAccepting())
			printf("***** ACCEPTING STATE *****\n");

		if(i >= B){
			break;
		}
		//add error status
	}
	printf("--\n");
}

void fireNonActionTransition(state* current){
	transition* trans = nullptr;
	while(true) {
		auto transitions = current->executables();
		trans = transition::sampleNonUniform(transitions);
		if(trans->action == "")
			current->apply(trans);
		else
			break;
	}
}

int launchExecutionMarkovChain(const fsm* automata, const TVL* tvl) {
	state* current = initState::createInitState(automata, tvl);
	fireNonActionTransition(current);
	unsigned long i = 0;
	//printf("**********************************\n");
	current->PRINT_STATE();

	graphVis = new stateToGraphViz(automata);

	graphVis->printGraphViz(current);

	std::vector<std::string> scheduler;
	
	std::ifstream myfile;

    //Should this file be hard-coded?
	myfile.open("mdp.sched");
	
	std::string myline;
	
	if ( myfile.is_open() ) {
		while ( myfile ) { // equivalent to myfile.good()
			std::getline (myfile, myline);
			scheduler.push_back(myline);
			std::cout << myline << '\n';
		}
	} else {
		std::cout << "Couldn't open file\n";
	}

	transition* trans = nullptr;

	do {

		auto sVariable = current->getVariable("s");
		int sValue = dynamic_cast<primitiveVariable*>(sVariable)->getValue();
		std::string schedValue = scheduler[sValue]; 
		
		auto transitions = current->executables();
		trans = transition::select(transitions, schedValue);
		assert(trans->action != "");
		current->apply(trans);

		fireNonActionTransition(current);

		//printf("--------------------------------------\n");
		//if(i % 3 == 0) {
		current->PRINT_STATE();
		graphVis->printGraphViz(current);
		//}

		if(current->isAccepting()) {
			std::cout << "accepting trace" << std::endl;
			return 0;
		}

		++i;
		if(i >= B){
			break;
		}
		//add error status
	} while(trans);

	printf("--\n");

	std::cout << "non accepting trace" << std::endl;
	return 1;
}

#define K 3

void findLasso(const fsm* automata, const TVL* tvl, size_t k_steps) {
	
	size_t i = 0;

	std::set<unsigned long> hashSet;

	state* current = initState::createInitState(automata, tvl);
	transition* trans = nullptr;

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
            } else {
                break; // No valid transition, exit the loop
            }
        } else {
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
void createStateSpaceBFS(const fsm* automata, const TVL* tvl) {
	std::stack<elementStack::element*> st;
	std::set<unsigned long> hm;
	state* init = initState::createInitState(automata, tvl);

	graphVis = new stateToGraphViz(automata);
	graphVis->printGraphViz(init);

	int depth = 0;

	st.push(new elementStack::element(init));
	hm.insert(init->hash());
	
	unsigned long i = 0;
	
	while(!st.empty()){

		auto& current = st.top();
		i++;

		printf("****************** current state ****************\n");
		current->s->PRINT_STATE();
		st.pop();
		depth = current->depth;
		
		auto nexts = current->s->Post();
		delete current;

		if(nexts.size() > 0) {
			printf("************* next possible states **************\n");
			++depth;
			for(auto& n : nexts) {

				n->PRINT_STATE();

				if(hm.find(n->hash()) != hm.end()) {
					printf("************* already visited state **************\n");
					delete n;
				} else {
					st.push(new elementStack::element(n, depth));
					hm.insert(n->hash());
					graphVis->printGraphViz(n, depth);
				}

				if(nexts.size() > 1) {
					printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
				}
			}
		} else {
			printf("************* end state **************\n");
		}
	}

	printf("number of states : %ld\n", i);

	delete graphVis;
}

/**
 * This function prints the state space for a given finite state machine using depth-first search to the console.
 * 
 * @param automata A pointer to the finite state machine to create the state space for.
 * @param tvl A pointer to the transition vector list for the finite state machine.
 */
void createStateSpaceDFS(const fsm* automata, const TVL* tvl) {

	std::stack<elementStack::element*> st;
	std::set<unsigned long> hm;
	state* init = (initState::createInitState(automata, tvl));

	graphVis = new stateToGraphViz(automata);

	int depth = 0;

	st.push(new elementStack::element(init));
	hm.insert(init->hash());
	
	unsigned long i = 0;
	
	while(!st.empty()){

		auto current = st.top();

		printf("****************** current state ****************\n");
		current->s->PRINT_STATE();
		
		if(!current->init) {
			current->Post = current->s->Post();
			current->init = true;
		}

		if(current->Post.size() > 0) {
			
			auto n = *current->Post.begin();
			current->Post.pop_front();	

			printf("************* pick next state **************\n");
			
			n->PRINT_STATE();

			if(hm.find(n->hash()) != hm.end()) {
				printf("************* already visited state **************\n");
				delete n;
			} else {
				st.push(new elementStack::element(n, depth));
				hm.insert(n->hash());
				++depth;
				i++;

				graphVis->printGraphViz(n, depth);
			}
			
			printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");

		} else {
			printf("************* end state **************\n");
			delete current;
			st.pop();
			--depth;
		}
	}

	printf("number of states : %ld\n", i);

	delete graphVis;
}

void createStateSpaceDFS_RR(const fsm* automata, const TVL* tvl) {
	std::stack<elementStack::element*> st;
	state* init = (initState::createInitState(automata, tvl));

	reachabilityRelation R;
	R.tvl = tvl;

	graphVis = new stateToGraphViz(automata);

	int depth = 0;

	R.getStatus(init);
	R.init(init);
	
	st.push(new elementStack::element(init));

	graphVis->printGraphViz(init, depth);
	
	unsigned long i = 0;
	
	while(!st.empty()){

		auto current = st.top();

		//printf("****************** current state ****************\n");
		//current->s->PRINT_STATE();
		
		if(!current->init) {
			current->Post = current->s->Post();
			assert(current->Post.size() > 0);
			current->init = true;
		}

		if(current->Post.size() > 0) {
			
			auto n = *current->Post.begin();
			current->Post.pop_front();

			//printf("************* pick next state **************\n");
			
			//n->PRINT_STATE();

			auto status = R.getStatus(n);
			R.update(n);

			graphVis->printGraphViz(n, depth);

			if(status == STATES_SAME_S1_VISITED) {
				//printf("************* already visited state **************\n");
				//graphVis->printGraphViz(n, depth);
				
				delete n;
			} else if (STATES_S1_NEVER_VISITED || STATES_SAME_S1_FRESH) {
				st.push(new elementStack::element(n, depth));
				
				//graphVis->printGraphViz(n, depth);

				++depth;
				i++;

				//graphVis->printGraphViz(n, depth);
			} else {
				assert(false);
			}
			
			//printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");

		} else {
			//printf("************* end state **************\n");
			delete current;
			st.pop();
			--depth;
		}
	}

	printf("number of states : %ld\n", i);

	delete graphVis;
}

static 		unsigned int _nbErrors = 0;				// Total number of encountered problems
static long unsigned int _nbStatesExplored = 1;		// Total of distinct states (without features) explored
static long unsigned int _nbStatesReExplored = 0;	// Total of states that had to be re-explored because new products were found to be able to reach them
static long unsigned int _nbStatesStops = 0;		// Total of states where exploration backtracked because they were already visited before
static long unsigned int _nbStatesExploredInner = 0;// As before, but for inner search.
static long unsigned int _nbStatesReExploredInner = 0;//As before, but for inner search.
static long unsigned int _nbStatesStopsInner = 0;	// As before, but for inner search.
static long unsigned int _depth = 0;				// Current exploration depth (inner and outer)

void ltlModelChecker::startNestedDFS(const fsm* automata, const TVL* tvl) {
    _nbStatesExplored = 0;
	_nbStatesReExplored = 0;
	printf("[startNestedDFS]\n");

	// Create initial state
	state* init = (initState::createInitState(automata, tvl));

	graphVis = new stateToGraphViz(automata);
	
	//graphVis->printGraphViz(init, _depth);

	//init->print();

	// Sanity checks
    auto neverClaim = init->getNeverClaim();
	auto neverTrans = neverClaim->transitions();
	if(!neverClaim || neverClaim->nullstate() || neverTrans.size() == 0) {
        printf("init->never is NULL\n");
		assert(false);
    }

	transition::erase(neverTrans);

    elementStack stack;
    stack.push(init);

	printf("state size : %lu\n", init->getSizeOf());

	if(outerDFS(stack) == 0) {
        printf("Property satisfied");
		printf(" [explored %lu states, re-explored %lu, stops %lu].\n", _nbStatesExplored, _nbStatesReExplored, _nbStatesStops);
		if(_nbStatesExploredInner != 0)
			printf("The inner search explored %lu states and re-explored %lu.\n", _nbStatesExploredInner, _nbStatesReExploredInner); 
	
	} else {
		/*auto _failProducts = R.getFailedProducts();
		auto _nbErrors = R.nbErrors;
		auto _allProductsFail = (tvl->getFeatureModelClauses() & ~_failProducts).IsZero();*/
		printf("\n");																	
		printf("Non Exhaustive search finished ");											
		printf(" [explored %lu states, re-explored %lu].\n",							
				_nbStatesExplored, _nbStatesReExplored);
		if(_nbStatesExploredInner != 0)
			printf("The inner search explored %lu states and re-explored %lu.\n", _nbStatesExploredInner, _nbStatesReExploredInner);

		//if(_nbErrors == 1) printf(" -  One problem found");								
		//else printf(" - %u problems were found", _nbErrors);							
		/*if(_allProductsFail /*|| isTautology(_failProducts)///) 								
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

byte ltlModelChecker::outerDFS(elementStack& stackOuter) {

    byte exhaustive = 0;

	R.getStatus(stackOuter.top()->s);
	R.init(stackOuter.top()->s);

	//graphVis->printGraphViz(stackOuter.top()->s);

	// Execution continues as long as the

	//  - stack is not empty
	//  - no error was found (except in the exhaustive case)
	while(!stackOuter.empty() && (!R.hasErrors() || exhaustive) /*&& !R.isComplete()*/) {

		auto current = stackOuter.top();
		//graphVis->printGraphViz(current->s);

		auto s_hash = current->s->hash();

		if(current->s->getErrorMask() & state::ERR_DEADLOCK) {

			printf("Found deadlock");
			printElementStack(stackOuter.stackElem);
			assert(false);	

		} else if(current->s->safetyPropertyViolation()) {
			// Safety property violated.
			// We have to pop two states off the stack to get to the violating state:
			//  -> the current top is a skip transition fired from an accepting state
			//  -> the state below that is the accepting state
			//  -> the state below that is the state that actually led to the accepting state to be reachable.
			//     i.e. this state is the actual violating state.

			printf("Safety property violated %lu.\n", s_hash);
			printElementStack(stackOuter.stackElem);

			R.addTraceViolation(current->s);

			stackOuter.pop();

            //auto newTop = stackOuter.top();
			//graphVis->printGraphViz(newTop->s);

            stackOuter.pop();

			//newTop = stackOuter.top();
			//graphVis->printGraphViz(newTop->s);

            stackOuter.pop();
			
			_depth -= 3;

		// Otherwise, the state can be explored (or exploration continue)

		} else {
			
			//current->s->print();
			//printf("    +-> exploring %lu...\n", s_hash);
			//current->setErrorStatus = _nbErrors;

			// ..., or there is a transition to be executed:
			if(current->Post.size() > 0) {

				
				//printf("    +-> peecking state %lu...\n", s_hash);

				auto s_ = *current->Post.begin();
				s_hash = s_->hash();

				current->Post.pop_front();

				//s_->print();

				//graphVis->printGraphViz(s_);

				//s_->print();

				if(s_->getErrorMask() & state::ERR_ASSERT_FAIL) {
					printf("Assertion at line %d violated", *s_->getOrigin()->lines.begin());
					
					R.addTraceViolation(current->s);
					
					delete s_;
					s_ = nullptr;

				} else {
					
					//get the status before update!
					auto status = R.getStatus(s_);
					//graphVis->printGraphViz(s_, _depth);
				
					if(status == STATES_SAME_S1_VISITED) {
						//printf("         - state %lu already visited.\n", s_hash);
						//s_->print();
						delete s_;

						_nbStatesStops++;
					
					} else {

						//graphVis->printGraphViz(s_, _depth);
						
						if(status == STATES_SAME_S1_FRESH) {

							// The state is not a new state:
							//printf("         - state %lu visited but features fresh, pushing on stack.\n", s_hash);

							//graphVis->printGraphViz(s_);

							_nbStatesReExplored++;

						} else if(status == STATES_S1_NEVER_VISITED){

							//graphVis->printGraphViz(s_, _depth);

							_nbStatesExplored++;

							//printf("         - state fresh %lu, pushing on stack.\n", s_hash);

						}

						R.update(s_);
						//assert(R.getStatus(s_) != status);

						_depth++;
						stackOuter.push(s_, _depth);
					}

				}

			} else if(current->Post.size() == 0) {
				
				s_hash = current->s->hash();

				//printf("    +-> all transitions of state %lu fired, acceptance check and backtracking...\n", s_hash);

				// Back these values up, the inner search will free current->state before returning

				if(current->s->isAccepting()) {
					_depth++;
					_nbStatesExploredInner++;
					
					//printf("    +-> found accepting state %lu, starting inner...\n", s_hash);
					elementStack stackInner;
					stackInner.push(current->s->deepCopy(), _depth);

					// error needs to be to the right, for otherwise lazy evaluation might cause the innerDFS call to be skipped
					R.setDFS(reachabilityRelation::DFS_INNER);
					innerDFS(stackInner, stackOuter);
					R.setDFS(reachabilityRelation::DFS_OUTER);
					// it will have been destroyed when the innerDFS backtracked for the last time
					//delete current->s;
				}
				
				//current->s = nullptr;
				stackOuter.pop();
				_depth--;
				//printf("    +-> State %lu erase from the hast table.\n", s_hash);
			}

		} // explore state
	} // end while

				
	// If error is true and we end up here, then we're in exhaustive mode. A summary has to be printed
	//if(error /* not needed: && exhaustive */
	
	assert(stackOuter.empty() || !exhaustive);
	while(!stackOuter.empty()) {
		stackOuter.pop();
	}

	//TVL::printBool(R.getFailedProducts());	
	return R.hasErrors();
}

byte ltlModelChecker::innerDFS(elementStack& stackInner, const elementStack& stackOuter) {

    byte exhaustive = 0;

	// Execution continues as long as the
	//  - stack is not empty
	//  - no error was found (except in the exhaustive case)
	while(!stackInner.empty() && (!R.hasErrors() || exhaustive) /*&& !R.isComplete()*/) {

		auto current = stackInner.top();
		auto s_hash = current->s->hash();
		
		//current->s->print();
		//printf("    +-> inner exploring %lu...\n", s_hash);
		//current->setErrorStatus = _nbErrors;
		
		if(current->s->getErrorMask() & state::ERR_DEADLOCK) {

			printf("Found deadlock");
			printElementStack(stackOuter.stackElem);
			assert(false);	

		}
				
		// If we have explored all transitions of the state (!current->E_never; see "struct stackElt"
		// in stack.h), we check whether the state is accepting and start a backlink search if it is;
		// otherwise just backtrack
		if(current->Post.size() == 0) {
			//printf("    +-> inner all transitions of state %lu fired, backtracking...\n", s_hash);
			stackInner.pop();
			_depth--;

		// ..., or there is a transition to be executed:
		} else if(current->Post.size() > 0) {

			//printf("    +-> inner peecking state %lu...\n", s_hash);

			auto s_ = *current->Post.begin();
			current->Post.pop_front();

			//s_->print();

			s_hash = s_->hash();
			bool onSt = stackOuter.isIn(s_hash);

			//graphVis->printGraphViz(s_);

			if(onSt || s_->getErrorMask() & state::ERR_ASSERT_FAIL) {

				if(onSt) {	
					printf("Property violated\n");
				} else {
					printf("Assertion at line %d violated", *s_->getOrigin()->lines.begin());
				}

				stackInner.push(s_, _depth+1);
				printElementStack(stackOuter.stackElem, stackInner.stackElem, s_);
				stackInner.pop();

				R.addTraceViolation(current->s);

			} else {
				
				//get the status before update!
				auto status = R.getStatus(s_);
				
				//graphVis->printGraphViz(s_, _depth);

				auto lastFoundIn = R.lastFoundIn(s_);
				//update put to inner if outer
				R.update(s_);
				assert(R.lastFoundIn(s_) == reachabilityRelation::DFS_INNER);

				if(status == STATES_SAME_S1_VISITED) {
					//printf("         - inner state %lu already visited.\n", s_hash);
					delete s_;

					_nbStatesStops++;
				
				} else if(status == STATES_SAME_S1_FRESH) {
					
					// The state is not a new state:
					if(lastFoundIn == reachabilityRelation::DFS_INNER) {
						//printf("                 - inner state %lu visited, but features fresh\n", s_hash);
						_nbStatesReExploredInner++;
					} else {
						//printf("                 - inner state %lu only visited during outer search\n", s_hash);
						_nbStatesExploredInner++;
					}
					//done by the reachability relation object logic
					//graphVis->printGraphViz(s_);

					_depth++;
					_nbStatesReExplored++;

					//will put to inner if it was outer
					stackInner.push(s_, _depth);
					
				} else {

					printElementStack(stackOuter.stackElem, stackInner.stackElem, s_);
					printf("Bug! The above state was found during the inner DFS but not during the outer! Aborting.\n");
					assert(false);
				}
				 // fresh state
				
				// no assert violation
			}
		} // fire post

	} // end while 
				
	// If error is true and we end up here, then we're in exhaustive mode. A summary has to be printed
	//if(error /* not needed: && exhaustive */
	
	while(!stackInner.empty())
		stackInner.pop();

	return R.hasErrors();
}

std::stack<elementStack::element*> reverse(const std::stack<elementStack::element*>& stack) {
	std::stack<elementStack::element*> reversed;
	auto copy = stack;
	while(!copy.empty()){
		reversed.push(copy.top());
		copy.pop();
	}
	return reversed;
}



void printElementStack(const std::stack<elementStack::element*>& outerStack, const std::stack<elementStack::element*>& innerStack, const state* loopBegin) {
	state* s = nullptr;
	unsigned int depth = 0;
	graphVis->setIn(stateToGraphViz::PREFIX);
	auto reverseStack = reverse(outerStack);
	std::cout << "\n - Stack trace:\n";
	bool inCycle = false;
	while(!reverseStack.empty()) {
		s = reverseStack.top()->s;
		depth = reverseStack.top()->depth;
		reverseStack.pop();
		if(loopBegin && loopBegin->hash() == s->hash()) {
			std::cout << "    -- Loop beings here --\n    --\n";
			s->print();
			graphVis->setIn(stateToGraphViz::CYCLE_BEGIN);
			graphVis->printGraphViz(s, depth);
			graphVis->setIn(stateToGraphViz::CYCLE);
			std::cout << "    -- Loop begin repeated in full:\n";
			inCycle = true;
			continue;
		}

		if(inCycle) {
			s->print();
			graphVis->printGraphViz(s, depth);
		}

		//s->print();
		//graphVis->printGraphViz(s, depth);
	}

	if(!loopBegin) {
		std::cout << "    -- Final state repeated in full:\n";
		s->print();
		//graphVis->printGraphViz(s, depth);
	}

	reverseStack = reverse(innerStack);
	while(!reverseStack.empty()) {
		auto top = reverseStack.top();
		top->s->print();
		graphVis->printGraphViz(top->s, top->depth);
		reverseStack.pop();
	}
	std::cout << "\n\n ****\n";
}