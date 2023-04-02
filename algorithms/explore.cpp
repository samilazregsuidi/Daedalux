#include "explore.hpp"

#include <stdio.h>
#include <algorithm>

#include "state.hpp"
#include "transition.hpp"
#include "process.hpp"

#include "initState.hpp"

//bad coupling?

#include "semantic.hpp"

#include "stateToGraphViz.hpp"

#define PRINT_STATE print

#define B 100

stateToGraphViz* graphVis = nullptr;

void launchExecution(const fsm* automata, const TVL* tvl) {
	state* current = initState::createInitState(automata, tvl);
	unsigned long i = 0;
	//printf("**********************************\n");
	current->PRINT_STATE();

	graphVis = new stateToGraphViz(automata);

	graphVis->printGraphViz(current);

	while(transition* trans = transition::sample(current->executables())){
		current->apply(trans);
		//printf("--------------------------------------\n");
		current->PRINT_STATE();
		graphVis->printGraphViz(current);

		if(current->isAccepting())
			printf("***** ACCEPTING STATE *****\n");

		if(i > B){
			break;
		}
		//add error status
	}
	printf("--\n");
}

#define K 3

void findLasso(const fsm* automata, const TVL* tvl, size_t k_steps) {
	
	size_t i = 0;

	std::set<unsigned long> hashSet;

	state* current = initState::createInitState(automata, tvl);
	transition* trans = nullptr;

	while(true) {

		//printf("**********************************\n");
		current->PRINT_STATE();
		graphVis->printGraphViz(current);

		auto hash = current->hash();
		if(hashSet.find(hash) == hashSet.end() || i++ < k_steps) {
			
			hashSet.insert(current->hash());
			
			if((trans = transition::sample(current->executables()))) {
				printf("..\n");
				current->apply(trans);
			} else 
				break;

		} else break;
		
	}
		
	printf("--\n");
}

#define D 1000

void createStateSpaceBFS(const fsm* automata, const TVL* tvl) {
	std::stack<element*> st;
	std::set<unsigned long> hm;
	state* init = (initState::createInitState(automata, tvl));

	graphVis = new stateToGraphViz(automata);

	graphVis->printGraphViz(init);

	int depth = 0;

	st.push(new element(init));
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
					st.push(new element(n, depth));
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

void createStateSpaceDFS(const fsm* automata, const TVL* tvl) {
	std::stack<element*> st;
	std::set<unsigned long> hm;
	state* init = (initState::createInitState(automata, tvl));

	graphVis = new stateToGraphViz(automata);

	int depth = 0;

	st.push(new element(init));
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
				st.push(new element(n, depth));
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

static 		unsigned int _nbErrors = 0;				// Total number of encountered problems
static long unsigned int _nbStatesExplored = 1;		// Total of distinct states (without features) explored
static long unsigned int _nbStatesReExplored = 0;	// Total of states that had to be re-explored because new products were found to be able to reach them
static long unsigned int _nbStatesStops = 0;		// Total of states where exploration backtracked because they were already visited before
static long unsigned int _nbStatesExploredInner = 0;// As before, but for inner search.
static long unsigned int _nbStatesReExploredInner = 0;//As before, but for inner search.
static long unsigned int _nbStatesStopsInner = 0;	// As before, but for inner search.
static long unsigned int _depth = 0;				// Current exploration depth (inner and outer)

void startNestedDFS(const fsm* automata, const TVL* tvl) {
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

    std::stack<element*> stack;
    stack.push(new element(init));

	if(outerDFS(stack) == 0) 
        printf("Property satisfied");

	delete graphVis;
}

int i = 0;

byte outerDFS(std::stack<element*>& stackOuter) {

	byte error = 0;
    byte exhaustive = 0;

	std::map<unsigned long, htState*> map;
	auto first = stackOuter.top();
	map[first->s->hash()] = stateToHTState(first->s);

	reachabilityRelation* R = new reachabilityRelation(OUTER, map);

	// Execution continues as long as the

	//  - stack is not empty
	//  - no error was found (except in the exhaustive case)
	while(!stackOuter.empty() && (!error || exhaustive)) {

		auto current = stackOuter.top();
		auto s_hash = current->s->hash();

		if(current->s->safetyPropertyViolation()) {
			// Safety property violated.
			// We have to pop two states off the stack to get to the violating state:
			//  -> the current top is a skip transition fired from an accepting state
			//  -> the state below that is the accepting state
			//  -> the state below that is the state that actually led to the accepting state to be reachable.
			//     i.e. this state is the actual violating state.
			delete current;
			stackOuter.pop();

            auto newTop = stackOuter.top();

			//graphVis->printGraphViz(newTop->s);

			delete newTop;

            stackOuter.pop();

			printf("Safety property violated %lu.\n", s_hash);
			printElementStack(stackOuter);

			error = 1;

			auto e = R->map.find(s_hash);
			assert(e != R->map.end());
			delete e->second;
			R->map.erase(e);
			printf("State %lu erase from the hast table.\n", s_hash);

			newTop = stackOuter.top();
			//graphVis->printGraphViz(newTop->s);
			delete newTop;

            stackOuter.pop();
			
			_depth -= 3;

		// Otherwise, the state can be explored (or exploration continue)

		} else {
			
			printf("    +-> exploring %lu...\n", s_hash);
			//current->setErrorStatus = _nbErrors;

			// If the element is uninitialised; the executable transitions have to be determined
			if(!current->init) {
				printf("    +-> initialising %lu...\n", s_hash);
				current->init = true;

				auto neverTs = current->s->getNeverClaim()->executables();
				if(neverTs.size() > 0) {
					transition::erase(neverTs);

					current->Post = current->s->Post();
						
					// Check for deadlocks
					if(current->Post.size() == 0) {
						printf("Found deadlock");
						//printElementStack(stackOuter);
						//error = 1;	
					}
				}
			}
			// ..., or there is a transition to be executed:
			if(current->Post.size() > 0) {

				auto s_ = *current->Post.begin();
				current->Post.pop_front();

				//graphVis->printGraphViz(s_);

				s_hash = s_->hash();

				//s_->print();

				if(s_->getErrorMask() & state::ERR_ASSERT_FAIL) {
					printf("Assertion at line %d violated", *s_->getOrigin()->lines.begin());
					
					error = 1;
					
					delete s_;
					s_ = nullptr;

				} else {
					
					auto foundIt = R->map.find(s_hash);
					if(foundIt != R->map.end()){
						
						auto comp = R->updateReachability(s_);
	
						if(comp == STATES_SAME_S1_VISITED) {
							printf("         - state %lu already visited.\n", s_hash);
							delete s_;
							s_ = nullptr;
							_nbStatesStops++;
						
						} else if(comp == STATES_SAME_S1_FRESH) {
							
							// The state was visited already, but the current copy is "fresher".
							// No need to insert it into the hash table, just update the feature expression

							// Important: PrevS_ can only be a state that was fully explored with the features
							// it has now. This is because:
							//  - it has been visited already (otherwise, it wouldn't be in the hashtab)
							//  - it is not a state on the current stack (otherwise, it wouldn't be fresh)
							// This means, that all states that could be visited with prevS_->features have
							// been visited already.  So, when we continue, we use s_->features and not
							// s_->features || prevS_->features.

							// The state is not a new state:
							printf("         - state %lu visited but features fresh, pushing on stack.\n", s_hash);
							//done by the reachability relation object logic
							
							//graphVis->printGraphViz(s_);

							_depth++;
							_nbStatesReExplored++;

							auto new_ = new element(s_, _depth);
							stackOuter.push(new_);
							
						}

					} else {

						//graphVis->printGraphViz(s_, _depth);

						_depth++;
						_nbStatesExplored++;

						printf("         - state fresh %lu, pushing on stack.\n", s_hash);
						map[s_hash] = stateToHTState(s_);
						auto new_ = new element(s_, _depth);
						stackOuter.push(new_);
						
					}

				} // fresh state
				// no assert violation

				// Simulate nested loop: when at the end of E, restart the E and advance the E_never
				// The deadlock test (E empty) is already done.  This also guarantees that E->value
				// is never NULL in the above apply(globalSymTab, mtypes, ).

			} else if(current->Post.size() == 0) {
				
				s_hash = current->s->hash();

				printf("    +-> all transitions of state %lu fired, acceptance check and backtracking...\n", s_hash);
				// Back these values up, the inner search will free current->state before returning

				if(current->s->isAccepting()) {
					_depth++;
					_nbStatesExploredInner++;
					
					printf("    +-> found accepting state %lu, starting inner...\n", s_hash);
					std::stack<element*> stackInner;
					auto accepting = new element(current->s->deepCopy(), _depth);
					stackInner.push(accepting);

					

					// error needs to be to the right, for otherwise lazy evaluation might cause the innerDFS call to be skipped
					error = innerDFS(stackInner, stackOuter, R->map) || error;
					// it will have been destroyed when the innerDFS backtracked for the last time
					//delete current->s;
					
				}
				
				//current->s = nullptr;
				delete current;
				stackOuter.pop();
				_depth--;

				auto htElem = R->map.find(s_hash);
				assert(htElem != R->map.end());
				delete htElem->second;
				R->map.erase(htElem);
				printf("State %lu erase from the hast table.\n", s_hash);
			}
				

		} // explore state
	} // end while

				
	// If error is true and we end up here, then we're in exhaustive mode. A summary has to be printed
	//if(error /* not needed: && exhaustive */
	
	assert(stackOuter.empty() || !exhaustive);
	while(!stackOuter.empty()) {
		delete stackOuter.top();
		stackOuter.pop();
	}

	for(auto htS : map)
		delete htS.second;

	delete R;

	return error;
}

byte innerDFS(std::stack<element*>& stackInner, const std::stack<element*>& stackOuter, std::map<unsigned long, htState*>& map) {

	byte error = 0;
    byte exhaustive = 0;

	reachabilityRelation* R = new reachabilityRelation(INNER, map);

	// Execution continues as long as the
	//  - stack is not empty
	//  - no error was found (except in the exhaustive case)
	while(!stackInner.empty() && (!error || exhaustive)) {

		auto current = stackInner.top();
		auto s_hash = current->s->hash();
		
		printf("    +-> exploring %lu...\n", s_hash);
		//current->setErrorStatus = _nbErrors;
		
		// If the element is uninitialised; the executable transitions have to be determined
		if(!current->init) {
			printf("    +-> initialising %lu...\n", s_hash);
			current->init = true;

			auto neverTs = current->s->getNeverClaim()->executables();
			if(neverTs.size() > 0) {
				transition::erase(neverTs);
				
				current->Post = current->s->Post();
					
				// Check for deadlocks
				if(current->Post.size() == 0) {
					//printf("Found deadlock");
					//printElementStack(stackOuter, stackInner);
					//error = 1;	
				}
			}
		}
				
		// If we have explored all transitions of the state (!current->E_never; see "struct stackElt"
		// in stack.h), we check whether the state is accepting and start a backlink search if it is;
		// otherwise just backtrack
		if(current->Post.size() == 0) {
			printf("    +-> all transitions of state %lu fired, backtracking...\n", s_hash);
			delete current;
			stackInner.pop();
			_depth--;

		// ..., or there is a transition to be executed:
		} else if(current->Post.size() > 0) {

			auto s_ = *current->Post.begin();
			current->Post.pop_front();

			s_hash = s_->hash();
			bool onSt = map.find(s_hash) != map.end();

			//graphVis->printGraphViz(s_);

			if(onSt || s_->getErrorMask() & state::ERR_ASSERT_FAIL) {

				auto top = new element(s_, _depth+1);
				stackInner.push(top);

				if(onSt) {	
					printf("Property violated\n");
					printElementStack(stackOuter, stackInner, s_);
				
				} else {
					printf("Assertion at line %d violated", *s_->getOrigin()->lines.begin());
					printElementStack(stackOuter, stackInner, s_);
				}

				error = 1;

				delete top;
				stackInner.pop();

			} else {
					
				auto s_Hash = s_->hash();
				auto foundIt = R->map.find(s_Hash);
				if(foundIt != R->map.end()){

					htState* prevS_ = foundIt->second;
					
					auto comp = R->updateReachability(s_);

					if(comp == STATES_SAME_S1_VISITED) {
						printf("         - state %lu already visited.\n", s_hash);
						delete s_;
						s_ = nullptr;
						_nbStatesStops++;
					
					} else if(comp == STATES_SAME_S1_FRESH) {
						
						// The state was visited already, but the current copy is "fresher".
						// No need to insert it into the hash table, just update the feature expression

						// Important: PrevS_ can only be a state that was fully explored with the features
						// it has now. This is because:
						//  - it has been visited already (otherwise, it wouldn't be in the hashtab)
						//  - it is not a state on the current stack (otherwise, it wouldn't be fresh)
						// This means, that all states that could be visited with prevS_->features have
						// been visited already.  So, when we continue, we use s_->features and not
						// s_->features || prevS_->features.
						

						// The state is not a new state:

						if(prevS_->foundIn == INNER) {
							printf("                 - state %lu visited, but features fresh\n", s_hash);
							_nbStatesReExploredInner++;
						} else {
							printf("                 - state %lu only visited during outer search\n", s_hash);
							prevS_->foundIn = INNER;
							_nbStatesExploredInner++;
						}
						//done by the reachability relation object logic
						//graphVis->printGraphViz(s_);

						_depth++;
						_nbStatesReExplored++;

						auto new_ = new element(s_, _depth);
						stackInner.push(new_);
						
					}

				} else {

					printElementStack(stackOuter, stackInner, s_);
					printf("Bug! The above state was found during the inner DFS but not during the outer! Aborting.\n");
					assert(false);

				} // fresh state
				// no assert violation
			}
		} // fire post

	} // end while 
				
	// If error is true and we end up here, then we're in exhaustive mode. A summary has to be printed
	//if(error /* not needed: && exhaustive */
	
	while(!stackInner.empty()) {
		delete stackInner.top();
		stackInner.pop();
	}

	delete R;

	return error;
}

reachabilityRelation::reachabilityRelation(DFS dfs, std::map<unsigned long, htState*>& map)
	: dfs(dfs)
	, map(map)
{}

reachabilityRelation::~reachabilityRelation() {
}

byte reachabilityRelation::updateReachability(state* s_) {
	auto s_Hash = s_->hash();
	auto foundIt = map.find(s_Hash);
	if(foundIt != map.end()) {

		current = foundIt->second;
		assert(current);
		s_->accept(this);
		
		return res;

	} else {
		return STATES_DIFF;
	}
}

void reachabilityRelation::visit(state* s) {
	assert(false);
}

void reachabilityRelation::visit(process* s) {
	assert(false);
}

void reachabilityRelation::visit(never* s) {	
}

void reachabilityRelation::visit(progState* s) {
	res = s->compare(current->hash);
}

void reachabilityRelation::visit(featStateDecorator* s) {
	
	auto feat = (dfs == OUTER)? &current->outerFeatures : &current->innerFeatures;

	res = s->compare(current->hash, *feat);
	if(res == STATES_DIFF || res == STATES_SAME_S1_VISITED) {
		return;
	}

	assert(res == STATES_SAME_S1_FRESH);
	
	auto negPrev = ~(*feat);
	*feat |= s->getFeatures();
	s->constraint(negPrev);
}

void reachabilityRelation::visit(compState* s) {
	auto comp = s->compare(current->hash);
	if(comp == STATES_DIFF) {
		res = STATES_DIFF;
		return;
	}

	else if (comp == STATES_SAME_S1_VISITED) {
	 	
		auto save = current;
		for(auto s : s->getSubStates()) {
			current = current->getSubHtState(s->hash());
			assert(current);
			s->accept(this);
			assert(res != STATES_DIFF);
			if(res == STATES_SAME_S1_FRESH) {
				comp = STATES_SAME_S1_FRESH;
			}
			current = save;
		}

		
		res = comp;
		return;
	} else {
		assert(false);
	}
}

stateToHTState::stateToHTState(state* s) {
	res = new htState(s->hash());
	s->accept(this);
}
    
void stateToHTState::visit(state* s) {
	assert(false); 
}

void stateToHTState::visit(process* s) {

}

void stateToHTState::visit(progState* s) {
	
}

void stateToHTState::visit(never* s) {
	
}

void stateToHTState::visit(compState* s) {
	auto save = res;
	for(auto s : s->getSubStates()) {
		htState* htS = new htState(s->hash());
		res->subStates.push_back(htS);
		res = htS;
		s->accept(this);
		res = save;
	}
}

void stateToHTState::visit(featStateDecorator* s) {

}

stateToHTState::operator htState*(void) const {
	return res;
}

std::stack<element*> reverse(const std::stack<element*>& stack) {
	std::stack<element*> reversed;
	auto copy = stack;
	while(!copy.empty()){
		reversed.push(copy.top());
		copy.pop();
	}
	return reversed;
}

void printElementStack(const std::stack<element*>& outerStack, const std::stack<element*>& innerStack, const state* loopBegin) {
	state* s = nullptr;
	unsigned int depth = 0;
	auto reverseStack = reverse(outerStack);
	std::cout << "\n - Stack trace:\n";	
	while(!reverseStack.empty()) {
		s = reverseStack.top()->s;
		depth = reverseStack.top()->depth;
		reverseStack.pop();
		if(loopBegin && loopBegin->hash() == s->hash()) {
			std::cout << "    -- Loop beings here --\n    --\n";
			s->print();
			//graphVis->printGraphViz(s, depth);
			std::cout << "    -- Loop begin repeated in full:\n";
		}
		s->print();
		graphVis->printGraphViz(s, depth);
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