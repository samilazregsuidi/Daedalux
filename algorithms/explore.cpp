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
		//current->printGraphViz(i);
		st.pop();
		
		
		auto nexts = current->Post();

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
	if(!neverClaim || !neverClaim->nullstate() || neverClaim->transitions().size() == 0) {
        assert(false);
        printf("init->never is NULL\n");
    }

    std::stack<element> stack;
    stack.push(init);

	if(outerDFS(stack) == 0) 
        printf("Property satisfied");
}

byte outerDFS(std::stack<element>& stackOuter) {

	byte error = 0;
    byte exhaustive = 1;
	byte temp; // TODO remove

	std::list<state*> Post;

	reachabilityRelation* R = new reachabilityRelation();

	// Execution continues as long as the
	//  - stack is not empty
	//  - no error was found (except in the exhaustive case)
	while(!stackOuter.empty() && (!error || exhaustive)) {

		auto current = stackOuter.top();
		auto neverClaim = current.s->getNeverClaim();

		if(neverClaim->endstate()) {
			// Safety property violated.
			// We have to pop two states off the stack to get to the violating state:
			//  -> the current top is a skip transition fired from an accepting state
			//  -> the state below that is the accepting state
			//  -> the state below that is the state that actually led to the accepting state to be reachable.
			//     i.e. this state is the actual violating state.
			stackOuter.pop();
			
            auto newTop = stackOuter.top();
            stackOuter.pop();

			printf("Safety property violated.\n");

			//STOP_ERROR("Safety property violated", ((ptStackElt) top(stackOuter))->state->features, stackOuter, NULL, NULL);
			error = 1;

			R->outerHT.erase(current.s->hash());

			newTop = stackOuter.top();
            stackOuter.pop();
			_depth = _depth - 3;

		// Otherwise, the state can be explored (or exploration continue)

		} else {
			printf("    +-> exploring...\n");
			//current->setErrorStatus = _nbErrors;

			// If the element is uninitialised; the executable transitions have to be determined
			if(!current.init) {
				printf("    +-> initialising...\n");
				
				//reusing Post of the last iteration
				if(Post.size()) {
					current.Post = Post;
					Post.clear();
				}
				else
					current.Post = current.s->Post();

				current.Post_save = current.Post;

				// Check for deadlocks
				if(current.Post.size() == 0) {
					printf("Found deadlock");
					error = 1;
				}
				
			}
			// If we have explored all transitions of the state (!current->E_never; see "struct stackElt"
			// in stack.h), we check whether the state is accepting and start a backlink search if it is;
			// otherwise just backtrack
			if(!current.Post.size()) {
				printf("    +-> all transitions of state fired, acceptance check and backtracking...\n");
				// Back these values up, the inner search will free current->state before returning

				auto accStateHash = current.s->hash();

				if(current.s->isAccepting()) {
					printf("    +-> found accepting, starting inner...\n");
					std::stack<element> stackInner;
					stackInner.push(current.s);

					_depth++;
					_nbStatesExploredInner++;

					error = innerDFS(stackOuter, stackInner) || error; // error needs to be to the right, for otherwise lazy evaluation might cause the innerDFS call to be skipped

					current.s = nullptr; // it will have been destroyed when the innerDFS backtracked for the last time
				}
				
				stackOuter.pop();
				R->outerHT.erase(accStateHash);
				_depth--;

			// ..., or there is a transition to be executed:
			} else if(current.Post.size()) {

				auto s_ = *current.Post.begin();

				Post = s_->Post();

				if(s_->getErrorMask() & state::ERR_ASSERT_FAIL) {
					char msg[40];
					printf("Assertion at line %d violated", s_->trans->lines.begin());
					error = 1;
					delete s_;
					s_ = nullptr;
					Post.clear();

				} else {
					
					auto s_Hash = s_->hash();
					auto foundIt = R->outerHT.find(s_Hash);
					if(foundIt != R->outerHT.end()){

						htState prevS_ = foundIt->second;
						state* candidate = prevS_.s;
						
						auto comp = R->updateReachability(s_);
	
						if(comp == STATES_SAME_S1_VISITED) {
							printf("         - state already visited.\n");
							delete s_;
							s_ = nullptr;
							for(auto p : Post) {
								delete p;
							}
							_nbStatesStops++;
						
						} else if(comp == STATES_SAME_S1_FRESH) {
							printf("         - state visited but features fresh, pushing on stack.\n");
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

							//done by the reachability relation object logic
							_nbStatesReExplored++;
						}

					} else {

						printf("         - state fresh, pushing on stack.\n");
						R->outerHT[s_Hash] = stateToHTState(s_);
						_nbStatesExplored++;
					}

				
	
					auto new_ = element(s_);

					stackOuter.push(new_);
					_depth++;
				} // fresh state
				// no assert violation

				// Simulate nested loop: when at the end of E, restart the E and advance the E_never
				// The deadlock test (E empty) is already done.  This also guarantees that E->value
				// is never NULL in the above apply(globalSymTab, mtypes, ).

				current.Post.pop_front();

			} // fire transition
		} // explore state
	} // end while

				
	// If error is true and we end up here, then we're in exhaustive mode. A summary has to be printed
	//if(error /* not needed: && exhaustive */
	//destroyStackElementStack(stackOuter, processTrans);
				
	return error;
}

byte innerDFS(std::stack<element*>& stackOuter, std::stack<element*>& stackInner) {

}

byte reachabilityRelation::updateReachability(state* s_) {
	auto s_Hash = s_->hash();
	auto foundIt = outerHT.find(s_Hash);
	if(foundIt != outerHT.end()) {

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

void reachabilityRelation::visit(progState* s) {
	res = s->compare(*current.s);
}

void reachabilityRelation::visit(featStateDecorator* s) {
	
	auto res = s->compare(*current.s, current.outerFeatures);
	if(res == STATES_DIFF || res == STATES_SAME_S1_VISITED) {
		return;
	}

	assert(res == STATES_SAME_S1_FRESH);

	auto negPrev = ~current.outerFeatures;
	current.outerFeatures |= s->getFeatures();
	s->constraint(negPrev);
}

void reachabilityRelation::visit(compState* s) {
	auto comp = s->compare(*current.s);
	if(comp == STATES_DIFF) {
		res = STATES_DIFF;
		return;
	}

	else if (comp == STATES_SAME_S1_VISITED) {
	 	
		auto save = current;
		for(auto s : s->getSubStates()) {
			current = current.getSubHtState(s->hash());
			assert(current.s);
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
	res = htState(s);
	s->accept(this);
}
    
void stateToHTState::visit(state* s) {
	assert(false); 
}

void stateToHTState::visit(process* s) {

}

void stateToHTState::visit(progState* s) {
	
}

void stateToHTState::visit(compState* s) {
	auto save = res;
	for(auto s : s->getSubStates()) {
		htState htS = s;
		res.subStates.push_back(s);
		res = s;
		s->accept(this);
		res = save;
	}
}

void stateToHTState::visit(featStateDecorator* s) {
	
}

stateToHTState::operator htState(void) const {
	return res;
}