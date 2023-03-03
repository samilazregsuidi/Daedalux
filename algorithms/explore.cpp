#include "explore.hpp"

#include <stdio.h>

#include "state.hpp"
#include "transition.hpp"
#include "process.hpp"

#include "initState.hpp"

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

	std::set<unsigned long> outerHT; 

	// Execution continues as long as the
	//  - stack is not empty
	//  - no error was found (except in the exhaustive case)
	while(!stackOuter.empty() && (!error || exhaustive)) {

		auto current = stackOuter.top();
		auto neverClaim = current.s->getNeverClaim();

		if(_nbErrors /*&& temp*/) {
			//DEBUG_PRINT("    +-> is known to violate, backtrack.\n");
			stackOuter.pop();
			//htOuterStatesRemove(current->state->payloadHash, current->state->payload, current->state->payloadSize);
			//destroyStackElement(current, processTrans);
			_depth--;
		// If the node of the never claim is NULL, then it had a loose end which was taken in the
		// previous transition.  This means that we are in a final state, which is assumed to be
		// accepting; hence:
		} else if(neverClaim->endstate()) {
			// Safety property violated.
			// We have to pop two states off the stack to get to the violating state:
			//  -> the current top is a skip transition fired from an accepting state
			//  -> the state below that is the accepting state
			//  -> the state below that is the state that actually led to the accepting state to be reachable.
			//     i.e. this state is the actual violating state.
			stackOuter.pop();
			
            auto newTop = stackOuter.top();
            stackOuter.pop();

			//STOP_ERROR("Safety property violated", ((ptStackElt) top(stackOuter))->state->features, stackOuter, NULL, NULL);
			error = 1;
			//htOuterStatesRemove(current->state->payloadHash, current->state->payload, current->state->payloadSize);

			newTop = stackOuter.top();
            stackOuter.pop();
			_depth = _depth - 3;

		// Otherwise, the state can be explored (or exploration continue)
		} else {
			printf("    +-> exploring...\n");
			//current->setErrorStatus = _nbErrors;

			// If the element is uninitialised; the executable transitions have to be determined
			if(!current.Post_save.size()) {
				printf("    +-> initialising...\n");
				if(Post.size()) {
					current.Post = Post;
					Post.clear();
				}
				else
					current.Post = current.s->Post();

				current.Post_save = current.Post;

				// Check for deadlocks
				if(!current.Post.size()) {
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
				outerHT.erase(accStateHash);
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
					//destroyProcTransList(E, processTrans);
					Post.clear();

				} else {
					state* prevS_ = nullptr;


					if(htVisitedStatesFind(s_->payloadHash, s_, DFS_OUTER, &prevS_)) {
						DEBUG_PRINT("         - state already visited.\n");
						stateDestroy(s_, false);
						s_ = NULL;
						destroyProcTransList(E, processTrans);
						E = NULL;
						_nbStatesStops++;

					} else {
						if(prevS_) {
							DEBUG_PRINT("         - state visited but features fresh, pushing on stack.\n");
							// The state was visited already, but the current copy is "fresher".
							// No need to insert it into the hash table, just update the feature expression

							// Important: PrevS_ can only be a state that was fully explored with the features
							// it has now. This is because:
							//  - it has been visited already (otherwise, it wouldn't be in the hashtab)
							//  - it is not a state on the current stack (otherwise, it wouldn't be fresh)
							// This means, that all states that could be visited with prevS_->features have
							// been visited already.  So, when we continue, we use s_->features and not
							// s_->features || prevS_->features.
							ptBoolFct negPrev = negateBool(prevS_->outerFeatures);
							if(s_->features == NULL) {
								destroyBool(prevS_->outerFeatures);
								prevS_->outerFeatures = getTrue();
							} else {
								prevS_->outerFeatures = addDisjunction(prevS_->outerFeatures, s_->features, 0, 1);

								#ifdef CHECK_TAUTOLOGY
									if(prevS_->outerFeatures && isTautology(prevS_->outerFeatures)) {
										destroyBool(prevS_->outerFeatures);
										prevS_->outerFeatures = getTrue();
									}
								#endif
							}
							s_->features = addConjunction(negPrev, s_->features, 0, 0);
							free(s_->payload);
							s_->payload = prevS_->payload;

							// The state is not a new state:
							_nbStatesReExplored++;
							STATS;

						} else {
							DEBUG_PRINT("         - state fresh, pushing on stack.\n");
							// The state was not visited at all
							htVisitedStatesInsert(s_->payloadHash, s_, DFS_OUTER);
							_nbStatesExplored++;
							STATS;

						}
						new = createStackElement(s_, _nbErrors);
#ifdef CEGAR
						new->trans = ((ptProcessTransition) current->E->value)->trans;
						new->accFeatures = conjunction;
#endif
						stackOuter = push(stackOuter, new);
						htOuterStatesInsert(s_->payloadHash, s_);
						_depth++;
					} // fresh state
				} // no assert violation
#ifdef CEGAR
				}
				else
					destroyBool(conjunction);
#endif
				// Simulate nested loop: when at the end of E, restart the E and advance the E_never
				// The deadlock test (E empty) is already done.  This also guarantees that E->value
				// is never NULL in the above apply(globalSymTab, mtypes, ).
				current->E_never = current->E_never->next;
				if(!current->E_never) {
					current->E = current->E->next;
					current->E_never = current->E_never_save;
				}
			} // fire transition
		} // explore state
	} // end while


	// If error is true and we end up here, then we're in exhaustive mode. A summary has to be printed
	if(error /* not needed: && exhaustive */) STOP_ERROR_GLOBAL;
	destroyStackElementStack(stackOuter, processTrans);
	return error;
}

byte innerDFS(std::stack<element*>& stackOuter, std::stack<element*>& stackInner) {

}