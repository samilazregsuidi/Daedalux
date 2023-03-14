#include "explore.hpp"

#include <stdio.h>

#include "state.hpp"
#include "transition.hpp"
#include "process.hpp"

#include "initState.hpp"

//bad coupling?

#include "semantic.hpp"

#define PRINT_STATE print

#define B 100

void launchExecution(const fsm* automata, const TVL* tvl) {
	state* current = initState::createInitState(automata, tvl);
	unsigned long i = 0;
	//printf("**********************************\n");
	current->PRINT_STATE();
	current->printGraphViz(i++);

	while(transition* trans = transition::sample(current->executables())){
		current->apply(trans);
		//printf("--------------------------------------\n");
		current->PRINT_STATE();
		current->printGraphViz(i++);

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
		current->printGraphViz(i);

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

void createStateSpace(const fsm* automata, const TVL* tvl) {
	std::stack<state*> st;
	std::map<unsigned long, state*> hm;
	state* current = initState::createInitState(automata, tvl);

	st.push(current);
	hm[current->hash()] = current;
	
	unsigned long i = 0;
	
	while(!st.empty()){

		current = st.top();
		i++;

		printf("****************** current state ****************\n");
		current->PRINT_STATE();
		current->printGraphViz(i);
		st.pop();
		
		
		auto nexts = current->Post();

		delete current;

		if(nexts.size() > 0) {
			printf("************* next possible states **************\n");
			for(auto n : nexts) {

				if(hm.find(n->hash()) != hm.end()) {
					printf("************* already visited state **************\n");
				} else {
					st.push(n);
					hm[n->hash()] = n;
				}
				n->PRINT_STATE();

				if(nexts.size() > 1) {
					printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
				}
			}
		} else {
			printf("************* end state **************\n");
		}
	}

	printf("number of states : %d\n", i);
}

void countStates(const fsm* automata, const TVL* tvl) {
	state* current = initState::createInitState(automata, tvl);

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
	state* init = initState::createInitState(automata, tvl);

	// Sanity checks
    auto neverClaim = init->getNeverClaim();
	if(!neverClaim || neverClaim->nullstate() || neverClaim->transitions().size() == 0) {
        printf("init->never is NULL\n");
		assert(false);
    }

    std::stack<element*> stack;
    stack.push(new element(init));

	if(outerDFS(stack) == 0) 
        printf("Property satisfied");
}

int i = 0;

byte outerDFS(std::stack<element*>& stackOuter) {

	byte error = 0;
    byte exhaustive = 1;

	std::map<unsigned long, htState*> map;
	auto first = stackOuter.top();
	map[first->s->hash()] = stateToHTState(first->s);

	reachabilityRelation* R = new reachabilityRelation(OUTER, map);

	// Execution continues as long as the
	//  - stack is not empty
	//  - no error was found (except in the exhaustive case)
	while(!stackOuter.empty() && (!error || exhaustive)) {

		auto current = stackOuter.top();
		auto neverClaim = current->s->getNeverClaim();
		auto s_hash = current->s->hash();

		current->s->printGraphViz(i++);

		if(neverClaim->endstate()) {
			// Safety property violated.
			// We have to pop two states off the stack to get to the violating state:
			//  -> the current top is a skip transition fired from an accepting state
			//  -> the state below that is the accepting state
			//  -> the state below that is the state that actually led to the accepting state to be reachable.
			//     i.e. this state is the actual violating state.
			//delete current;
			stackOuter.pop();

            auto newTop = stackOuter.top();
			newTop->s->printGraphViz(i++);
			//delete newTop;

            stackOuter.pop();

			printf("Safety property violated %lu.\n", s_hash);

			error = 1;

			auto e = R->map.find(s_hash);
			assert(e != R->map.end());

			//delete e->second;
			R->map.erase(e);
			printf("State %lu erase from the hast table.\n", s_hash);

			newTop = stackOuter.top();
			newTop->s->printGraphViz(i++);
			//delete newTop;

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
				
				auto neverPost = neverClaim->executables();
				if(!(neverPost.size() == 0)) {
					current->Post = current->s->Post();
					//current->Post_save = current->Post;

					// Check for deadlocks
					if(current->Post.size() == 0) {
						printf("Found deadlock");
						error = 1;	
					}
				}
				
			}
			// If we have explored all transitions of the state (!current->E_never; see "struct stackElt"
			// in stack.h), we check whether the state is accepting and start a backlink search if it is;
			// otherwise just backtrack
			if(current->Post.size() == 0) {
				
				s_hash = current->s->hash();

				printf("    +-> all transitions of state %lu fired, acceptance check and backtracking...\n", s_hash);
				// Back these values up, the inner search will free current->state before returning

				if(current->s->isAccepting()) {
					printf("    +-> found accepting state %lu, starting inner...\n", s_hash);
					std::stack<element*> stackInner;
					stackInner.push(new element(current->s));

					_depth++;
					_nbStatesExploredInner++;

					// error needs to be to the right, for otherwise lazy evaluation might cause the innerDFS call to be skipped
					error = innerDFS(stackInner, R->map) || error;
					// it will have been destroyed when the innerDFS backtracked for the last time 
					current->s = nullptr;
				}
				
				//delete current;
				stackOuter.pop();
				_depth--;

				auto htElem = R->map.find(s_hash);
				assert(htElem != R->map.end());
				//delete htElem->second;
				R->map.erase(htElem);
				printf("State %lu erase from the hast table.\n", s_hash);
				

			// ..., or there is a transition to be executed:
			} else if(current->Post.size() > 0) {

				auto s_ = *current->Post.begin();
				s_hash = s_->hash();

				s_->printGraphViz(i++);

				if(s_->getErrorMask() & state::ERR_ASSERT_FAIL) {
					printf("Assertion at line %d violated", *s_->getOrigin()->lines.begin());
					
					error = 1;
					
					//delete s_;
					s_ = nullptr;

				} else {
					
					auto foundIt = R->map.find(s_hash);
					if(foundIt != R->map.end()){

						htState* prevS_ = foundIt->second;
						state* candidate = prevS_->s;
						
						auto comp = R->updateReachability(s_);
	
						if(comp == STATES_SAME_S1_VISITED) {
							printf("         - state %lu already visited.\n", s_hash);
							//delete s_;
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

							auto new_ = new element(s_);
							stackOuter.push(new_);
							_depth++;
							_nbStatesReExplored++;
						}

					} else {

						printf("         - state fresh %lu, pushing on stack.\n", s_hash);
						R->map[s_hash] = stateToHTState(s_);
						auto new_ = new element(s_);
						stackOuter.push(new_);
						_depth++;
						_nbStatesExplored++;
					}

				} // fresh state
				// no assert violation

				// Simulate nested loop: when at the end of E, restart the E and advance the E_never
				// The deadlock test (E empty) is already done.  This also guarantees that E->value
				// is never NULL in the above apply(globalSymTab, mtypes, ).

				current->Post.pop_front();

			} // fire post
		} // explore state
	} // end while

				
	// If error is true and we end up here, then we're in exhaustive mode. A summary has to be printed
	//if(error /* not needed: && exhaustive */
	
	while(!stackOuter.empty()) {
		delete stackOuter.top();
		stackOuter.pop();
	}

	for(auto htS : map)
		delete htS.second;

	delete R;

	return error;
}

byte innerDFS(std::stack<element*>& stackInner, std::map<unsigned long, htState*>& map) {

	byte error = 0;
    byte exhaustive = 1;

	reachabilityRelation* R = new reachabilityRelation(INNER, map);

	// Execution continues as long as the
	//  - stack is not empty
	//  - no error was found (except in the exhaustive case)
	while(!stackInner.empty() && (!error || exhaustive)) {

		auto current = stackInner.top();
		auto neverClaim = current->s->getNeverClaim();
		auto s_hash = current->s->hash();

		current->s->printGraphViz(i++);
		
		printf("    +-> exploring %lu...\n", s_hash);
		//current->setErrorStatus = _nbErrors;

		// If the element is uninitialised; the executable transitions have to be determined
		if(!current->init) {
			printf("    +-> initialising %lu...\n", s_hash);
			current->init = true;
			
			auto neverPost = neverClaim->executables();
			if(!(neverPost.size() == 0)) {
				current->Post = current->s->Post();
				//current->Post_save = current->Post;

				// Check for deadlocks
				if(current->Post.size() == 0) {
					printf("Found deadlock");
					error = 1;	
				}
			
			}
		}
		// If we have explored all transitions of the state (!current->E_never; see "struct stackElt"
		// in stack.h), we check whether the state is accepting and start a backlink search if it is;
		// otherwise just backtrack
		if(!current->Post.size()) {
			printf("    +-> all transitions of state %lu fired, backtracking...\n", s_hash);
			//delete current;
			stackInner.pop();
			_depth--;

		// ..., or there is a transition to be executed:
		} else if(current->Post.size() > 0) {

			state* s_ = *current->Post.begin();
			s_hash = s_->hash();
			bool onSt = map.find(s_hash) != map.end();

			s_->printGraphViz(i++);

			if(onSt || s_->getErrorMask() & state::ERR_ASSERT_FAIL) {

				if(onSt) {	
					printf("Property violated\n");
				
				} else {
					printf("Assertion at line %d violated", *s_->getOrigin()->lines.begin());
				}

				error = 1;

				//delete s_;
				s_ = nullptr;

			} else {
					
				auto s_Hash = s_->hash();
				auto foundIt = R->map.find(s_Hash);
				if(foundIt != R->map.end()){

					htState* prevS_ = foundIt->second;
					state* candidate = prevS_->s;
					
					auto comp = R->updateReachability(s_);

					if(comp == STATES_SAME_S1_VISITED) {
						printf("         - state %lu already visited.\n", s_hash);
						//delete s_;
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

						auto new_ = new element(s_);
						stackInner.push(new_);
						_depth++;
						_nbStatesReExplored++;
					}

				} else {

					printf("Bug! The above state was found during the inner DFS but not during the outer! Aborting.\n");

				} // fresh state
				// no assert violation

				// Simulate nested loop: when at the end of E, restart the E and advance the E_never
				// The deadlock test (E empty) is already done.  This also guarantees that E->value
				// is never NULL in the above apply(globalSymTab, mtypes, ).
			}

			current->Post.pop_front();
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
	res = s->compare(*current->s);
}

void reachabilityRelation::visit(featStateDecorator* s) {
	
	auto feat = (dfs == OUTER)? &current->outerFeatures : &current->innerFeatures;

	auto res = s->compare(*current->s, *feat);
	if(res == STATES_DIFF || res == STATES_SAME_S1_VISITED) {
		return;
	}

	assert(res == STATES_SAME_S1_FRESH);
	
	auto negPrev = ~(*feat);
	*feat |= s->getFeatures();
	s->constraint(negPrev);
}

void reachabilityRelation::visit(compState* s) {
	auto comp = s->compare(*current->s);
	if(comp == STATES_DIFF) {
		res = STATES_DIFF;
		return;
	}

	else if (comp == STATES_SAME_S1_VISITED) {
	 	
		auto save = current;
		for(auto s : s->getSubStates()) {
			current = current->getSubHtState(s->hash());
			assert(current->s);
			s->accept(this);
			assert(res != STATES_DIFF);
			if(res == STATES_SAME_S1_FRESH)
				comp = STATES_SAME_S1_FRESH;
			current = save;
		}
		
		res = comp;
		return;
	} else {
		assert(false);
	}
}

stateToHTState::stateToHTState(state* s) {
	res = new htState(s);
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
		htState* htS = new htState(s);
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