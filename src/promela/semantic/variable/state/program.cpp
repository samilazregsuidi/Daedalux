#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "program.hpp"

#include "process.hpp"

#include "ast.hpp"
#include "automata.hpp"

#include "variable.hpp"
#include "channelVar.hpp"
#include "payload.hpp"
#include "variable.hpp"

#include "progTransition.hpp"
#include "rendezVousTransition.hpp"
#include "threadTransition.hpp"

#include "stateVisitor.hpp"

/**
 * Adds the global variables in the memory chunk.
 *
 * Does not set the payloadHash.
 */

program::program(const fsm* stateMachine, const std::string& name) 
	: state(variable::V_PROG, name)
	, globalSymTab(stateMachine->getGlobalSymTab())
	, stateMachine (stateMachine)
	, pidCounter(0)
	, lastStepPid(0)
	, nbProcesses(0)
	, handShakeChan(nullptr)
	, handShakeProc(nullptr)
	, exclusiveProc(nullptr)
	, timeout(false)
{
}

program::program(const program& other)
	: state(other)
	, globalSymTab(other.globalSymTab)
	, stateMachine(other.stateMachine)
	, pidCounter(other.pidCounter)
	, lastStepPid(other.lastStepPid)
	, nbProcesses(other.nbProcesses)
	, handShakeChan(nullptr)
	, handShakeProc(nullptr)
	, exclusiveProc(nullptr)
	, timeout(other.timeout)
{
	if(other.handShakeChan) {
		handShakeChan = get<channel*>(other.handShakeChan->getLocalName());
		assert(handShakeChan);
	}

	if(other.handShakeProc) {
		handShakeProc = get<process*>(other.handShakeProc->getName());
		assert(handShakeProc);
	}

	if(other.exclusiveProc) {
		exclusiveProc = get<process*>(other.exclusiveProc->getName());
		assert(exclusiveProc);
	}

  assert(other.getVariables().size() == getVariables().size());
  assert(getVariables().size());
}

program* program::deepCopy(void) const {
	program* copy = new program(*this);
	//auto newScope = deepCopy();
	//newScope->setPayload(getPayload()->copy());
	//copy->assign(newScope);
	return copy;
}

void program::init(void)
{

  state::init();
  // No process is executing something atomic
  // getPayload()->setValue(OFFSET_EXCLUSIVITY_VAR, NO_PROCESS);
  // No rendezvous has been requested.
  // getPayload()->setValue(OFFSET_HANDSHAKE_VAR, NO_HANDSHAKE);
}

void program::assign(const variable * sc)
{

	if(handShakeChan) {
		handShakeChan = sc->get<channel*>(handShakeChan->getLocalName());
		assert(handShakeChan);
	}
	if(handShakeProc) {
		handShakeProc = sc->get<process*>(handShakeProc->getName());
		assert(handShakeProc);
	}
	if(exclusiveProc) {
		exclusiveProc = sc->get<process*>(exclusiveProc->getName());
		assert(exclusiveProc);
	}
}

bool program::operator==(const variable* other) const {
	auto var = dynamic_cast<const program*>(other);
	if(var) {
		if(lastStepPid != var->lastStepPid)
			return false;
		if(timeout != var->timeout)
			return false;

		if(reinterpret_cast<long>(handShakeChan) ^ reinterpret_cast<long>(var->handShakeChan))
			return false;
		else if (handShakeChan && var->handShakeChan && handShakeChan->getLocalName() != var->handShakeChan->getLocalName())
			return false;
		

		if(reinterpret_cast<long>(handShakeProc) ^ reinterpret_cast<long>(var->handShakeProc))
			return false;
		 else if (handShakeProc && var->handShakeProc && handShakeProc->getName() != var->handShakeProc->getName()) 
			return false;

		if(reinterpret_cast<long>(exclusiveProc) ^ reinterpret_cast<long>(var->exclusiveProc))
			return false;
		else if (exclusiveProc && var->exclusiveProc && exclusiveProc->getName() != var->exclusiveProc->getName()) 
			return false;
		

	} else
		return false;

	return variable::operator==(other);
}

bool program::operator!=(const variable* other) const {
	return !(*this == other);
}

state* program::operator=(const variable* other) {
	variable::operator=(other);
	auto var = dynamic_cast<const program*>(other);
	if(var) {
		assert(globalSymTab == var->globalSymTab);
		assert(stateMachine == var->stateMachine);
		
		pidCounter = var->pidCounter;
		
		lastStepPid = var->lastStepPid;
		nbProcesses = var->nbProcesses;
		
		if(var->handShakeChan) {
			handShakeChan = var->get<channel*>(handShakeChan->getLocalName());
			assert(handShakeChan);
		} else 
			handShakeChan = nullptr;

		if(var->handShakeProc) {
			handShakeProc = var->get<process*>(handShakeProc->getName());
			assert(handShakeProc);
		} else 
			handShakeProc = nullptr;

		if(var->exclusiveProc) {
			exclusiveProc = var->get<process*>(exclusiveProc->getName());
			assert(exclusiveProc);
		} else 
			exclusiveProc = nullptr;

		timeout = var->timeout;

	} else {
		assert(false);
	}
	return this;
}

/**
 * Returns the list of all the transitions of the state.
 */

std::list<transition*> program::transitions(void) const {
	std::list<transition*> res;
	for(auto p : getProcs())
		res.merge(p->transitions());
	return res;
}

/**
 * Frees the memory used by a given state. It does NOT free any symbol tables, FSM or mtypes list.
 * However, it DOES free:
 *  - the memory chunk,
 *  - all state procs of active processes,
 *  - the state proc of the never claim (if any),
 *  - all channel references,
 *
 * If the keepPayloadAndFeatures parameter is false, it also frees:
 *  - boolean formula and
 *  - the state structure itself.
 *
 * This parameter is only true when destroying a stack element where the payload and boolean function
 * are still used in the visited states hashtable.
 */

program::~program() {}

/*
 * STATE COMPARISON
 * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * Compares s1 a newly reached state
 *     with s2 a state known to be reachable
 * to see whether s1 is a state that was already visited.
 *
 * When s1 was not yet visited, then we say it's "fresh".
 *
 * Returns:
 * 	- STATES_DIFF 			 if s1 and s2 are totally different states, meaning s1 is fresh.
 * 	- STATES_SAME_S1_VISITED if s1 and s2 are identical but s2 is reachable by more products; hence, s1 adds nothing new
 *  - STATES_SAME_S1_FRESH	 if s1 and s2 are identical but s1 has products that were not explored with s2; hence, s1 is
 * fresh
 */
/*

byte program::compare(const state& s2) const {

        if(!(*payLoad == *s2.payLoad))
                return STATES_DIFF;

        // Now that we know both states are identical, we check whether:
        //  s1 -> s2
        //
        // If this holds, then s1 is reachable in less products, which means
        //                that it can be considered as visited.
        // It not,        then s1 is reachable by at least one product that
        //                was not previously explored, so it contains some
        //                fresh info, and exploration must continue.

        // Convention: nullptr means 'true'.
        if(!s2Features) return STATES_SAME_S1_VISITED;
        if(!s1->features) return STATES_SAME_S1_FRESH;	// Here we do not check the case in which s2->features != nullptr but
still a tautology;
                                                                                                        // There is a
compilation parameter CHECK_TAUTOLOGY that can be set to check for
                                                                                                        // tautologies before
they end up here.

        if(implies(s1->features, s2Features)) return STATES_SAME_S1_VISITED;

        return STATES_SAME_S1_FRESH;
}*/

void program::print(void) const {
	variable::print();
	std::cout << "Exclusivity : " << (exclusiveProc? exclusiveProc->getName() : "None") << std::endl;
	printf("prob : %lf\n", prob);
	if(actions.size()){
		printf("scheduler : ");
		for(auto a : actions)
			printf(" %s, ", a.c_str());
	}
}

void program::printCSV(std::ostream & out) const
{
  variable::printCSV(out);
  out << prob << ",";
  for (auto a : actions)
    out << a << ",";
}

void program::printCSVHeader(std::ostream & out) const
{
  variable::printCSVHeader(out);
  out << "prob,";
  for (auto a : actions)
    out << a << ",";
}

/*
void program::printGraphViz(unsigned long i) const {
        std::ofstream stateFile;
        //stateFile.open("trace/" + std::to_string(hash()));
        stateFile.open("trace/" + std::to_string(i) + ".dot");

        std::list<const fsmNode*> locs;
        for(auto p : getProcs())
                locs.push_back(p->getFsmNodePointer());

        std::list<const fsmEdge*> edges;
        if(auto progTrans = dynamic_cast<const programTransition*>(origin)) {
                edges.push_back(progTrans->getEdge());
                auto t = progTrans;

                for(auto r : t->getResponses()){
                        edges.push_back(dynamic_cast<const programTransition*>(r)->getEdge());
                }
        }

        stateMachine->printGraphVisWithLocations(stateFile, locs, edges);
        stateFile.close();
}*/

std::list<process*> program::getProcs(void) const {
	return getAll<process*>();
}
/**
 * Returns the stateMask of a given pid.
 */
process* program::getProc(ubyte pid) const {
	auto procs = getProcs();
	for(auto proc : procs)
		if(proc->getPid() == pid)
			return proc;
	return nullptr;
}

state * program::getNeverClaim(void) const { return parent ? dynamic_cast<state *>(parent)->getNeverClaim() : nullptr; }

bool program::safetyPropertyViolation(void) const { return false; }

/**
 * Reserves some memory for the proctype variables in the memory chunk.
 * Returns the pid of the newly created process.
 *
 * Does not change the payloadHash.
 */
void program::addProcess(process * proc)
{

  if (nbProcesses >= MAX_PROCESS) {
    throw std::runtime_error("Cannot instantiate more than " + std::to_string(MAX_PROCESS) + " processes.");
  }

  proc->setPid(pidCounter++);
  _addVariable(proc);

  nbProcesses++;
}

/**
 * Defines the never claim of the execution.
 *
 * Does not change the payloadHash.
 */
/*
process* program::addNever(const neverSymNode* neverSym) {

        never = new process(neverSym, stateMachine->getFsmWithName(neverSym->getName()), -2);
        _addVariable(never);

        nbNeverClaim++;

        return never;
}
*/

/*******************************************************************************************************/

const process* program::getExclusiveProc(void) const {
	return exclusiveProc;
}

ubyte program::getExclusiveProcId(void) const {
	return getExclusiveProc()? getExclusiveProc()->getPid() : NO_PROCESS;
}

bool program::hasExclusivity(void) const {
	return getExclusiveProc() != nullptr;
}

void program::resetExclusivity(void) const {
	setExclusivity(NO_PROCESS);
}

void program::setExclusivity(const process* proc) const {
	exclusiveProc = proc;
	//getPayload()->setValue<byte>(OFFSET_EXCLUSIVITY_VAR, (proc? proc->getPid() : NO_PROCESS));
}

void program::setExclusivity(ubyte pid) const {
	setExclusivity(getProc(pid));
}

bool program::requestHandShake(const std::pair<const channel*, const process*>& handShake) const {
	if(!hasHandShakeRequest()) {
		setHandShake(handShake);
		return true;
	} return false; 
}

void program::setHandShake(const std::pair<const channel*, const process*>& handShake) const {
	assert((handShake.first && handShake.first) || (!handShake.first && !handShake.first));
	handShakeChan = handShake.first;
	handShakeProc = handShake.second;
	//getPayload()->setValue<int>(OFFSET_HANDSHAKE_VAR, (handShakeChan? handShakeChan->getVariableId() : NO_HANDSHAKE));
}

/*void program::setHandShake(unsigned int cid) const {

}*/

std::pair<const channel *, const process *> program::getHandShakeRequest(void) const { return {handShakeChan, handShakeProc}; }

unsigned int program::getHandShakeRequestId(void) const
{
  return hasHandShakeRequest() ? getHandShakeRequest().first->getVariableId() : NO_HANDSHAKE;
}

const channel * program::getHandShakeRequestChan(void) const { return handShakeChan; }

const process * program::getHandShakeRequestProc(void) const { return handShakeProc; }

bool program::hasHandShakeRequest(void) const { return handShakeChan != nullptr; }

void program::resetHandShake(void) const
{
  handShakeChan = nullptr;
  handShakeProc = nullptr;
  // getPayload()->setValue<int>(OFFSET_HANDSHAKE_VAR, NO_HANDSHAKE);
}

bool program::getTimeoutStatus(void) const { return timeout; }

/**
 * Returns a list of all the executable transitions (for all the processes).
 * EFFECTS: None. (It CANNOT have any!)
 * WARNING:
 * 	In the end, does NOT (and must NEVER) modify the state payload.
 */
std::list<transition *> program::executables(void) const
{

  std::list<transition *> execs;

	auto exclusivity = getExclusiveProc();
	auto handShake = getHandShakeRequest();

  for (auto proc : getProcs()) {
    auto Ts = proc->executables();
    // assert(std::fabs([=](){ double resProb = 0.0; for(auto t : Ts) resProb += t->prob; return resProb; }() - (Ts.size() ? 1.0
    // : 0.0)) < std::numeric_limits<double>::epsilon());
    /*for(auto t : Ts)
            t->prob /= getProcs().size();*/

    for (auto t : Ts) {
      execs.push_back(t);
    }
  }

  // assert(std::fabs([=](){ double resProb = 0.0; for(auto t : execs) resProb += t->prob; return resProb; }() - (execs.size()
  // ? 1.0 : 0.0)) < std::numeric_limits<double>::epsilon());

  if (execs.size() == 0) {

    if (hasExclusivity()) {
      resetExclusivity();
      execs = executables();
    }
    else if (timeout == false) {
      timeout = true;
      execs = executables();
      timeout = false;
    }
  }

  setExclusivity(exclusivity);
  setHandShake(handShake);

  return execs;
}

/**
 * Executes a statement and returns the new reached state. The transition must be executable.
 * The preserve parameter controls whether or not the state that is passed is preserved.
 *
 * The features expression of the processTransition is not modified. The value of this expression is
 * copied into the new state. Thus, when this state is destroyed, the features expression of the
 * processTransition is not deleted.
 *
 * assertViolation is a return value set to true in case the statement on the transition was an assert
 * that evaluated to false.
 */
void program::apply(transition* trans) {

	auto rdvTrans = dynamic_cast<const rendezVousTransition*>(trans);
	if(rdvTrans) {
		assert(rdvTrans->getQuestion() != nullptr);
		auto questionTrans = dynamic_cast<threadTransition*>(rdvTrans->getQuestion());
		assert(questionTrans);
		//warning if "different" procs have the same pid i.e., dynamic proc creation
		//not sure about that

		auto proc = getProc(questionTrans->getThread()->getPid());
		assert(proc);

		proc->apply(rdvTrans->getQuestion());

		auto responseTrans = dynamic_cast<threadTransition*>(rdvTrans->getResponse());
		if(responseTrans) {
			auto responseProc = getProc(responseTrans->getThread()->getPid());
			responseProc->apply(rdvTrans->getResponse());
		}
		prob *= trans->prob;

	} else {

		auto progTrans = dynamic_cast<programTransition*>(trans);
		auto procTrans = dynamic_cast<threadTransition*>(progTrans->getProcTrans()); 
		assert(procTrans);

		//warning if "different" procs have the same pid i.e., dynamic proc creation
		//not sure about that

		auto proc = getProc(procTrans->getThread()->getPid());
		assert(proc);

		proc->apply(procTrans);

		prob *= trans->prob;
	}

	assert(!getProc(lastStepPid)->isAtomic() || getExclusiveProcId() == lastStepPid);

	origin = trans;
	assert(trans->dst == nullptr);
	trans->dst = this;
}

bool program::nullstate(void) const
{
  auto processes = getProcs();
  return std::all_of(processes.begin(), processes.end(), [](const process * p) { return p->nullstate(); });
}

bool program::endstate(void) const
{
  auto processes = getProcs();
  return std::all_of(processes.begin(), processes.end(), [](const process * p) { return p->endstate(); });
}

bool program::isAccepting(void) const { return false; }

bool program::isAtomic(void) const
{
  auto processes = getProcs();
  return std::any_of(processes.begin(), processes.end(), [](const process * p) { return p->isAtomic(); });
}

void program::accept(stateVisitor * visitor) { visitor->visit(this); }