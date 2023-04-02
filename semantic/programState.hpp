#ifndef PROGRAM_STATE_H
#define PROGRAM_STATE_H

#include "state.hpp"

#include "symbols.hpp"
#include "automata.hpp"

class fsm;
class process;
class channel;
class payload;
class variable;

// Bytes needed to record the system variables: exclusive and handshake.
// For any channel, its offset is positive. Thus, we use the value -1 to specify the absence of rendezvous request.
#define NO_HANDSHAKE (int)0
#define HANDSHAKE_VAR (int)0 //T_INT
#define OFFSET_HANDSHAKE_VAR 0
#define SIZE_HANDSHAKE_VAR (sizeof(OFFSET_HANDSHAKE_VAR))

// For any process, its pid is between 0 and 254. Thus, we use the value 255 to specify absence of a process
#define NO_PROCESS (ubyte)255 // T_BYTE
#define MAX_PROCESS (ubyte)254 // T_BYTE
#define EXCLUSIVITY_VAR (ubyte)0 //T_BYTE
#define OFFSET_EXCLUSIVITY_VAR SIZE_HANDSHAKE_VAR
#define SIZE_EXCLUSIVITY_VAR (sizeof(EXCLUSIVITY_VAR))

// State
class progState : public state {
public:

	friend class process;

	progState(const fsm* stateMachine, const std::string& name = ""); // Creates the initial state by setting all variables' value in the payload. Does not set the payloadHash.

	progState(const progState* other);

	progState(const progState& other);

	//state(const state& s) = default;

	progState* deepCopy(void) const override;

	void assign(const variable* sc) override;
	/**
	 * Frees the memory used by a given state. It does NOT free any symbol tables, FSM or mtypes list.
	 * However, it DOES free:
	 *  - the memory chunk,
	 *  - all state masks of active processes,
	 *  - the state mask of the never claim (if any),
	 *  - all channel references,
	 *
	 * If the keepPayloadAndFeatures parameter is false, it also frees:
	 *  - boolean formula and
	 *  - the state structure itself.
	 *
	 * This parameter is only true when destroying a stack element where the payload and boolean function
	 * are still used in the visited states hashtable.
	 */
	virtual ~progState();

	void init(void) override;

	/*
	* Creates a new process and returns its pid.
	* Reserves some memory for the proctype variables in the memory chunk and initializes the value of these variables.
	* Does not change the payloadHash.
	*/
	void addProcess(process* proc);

	//void addProcess(const ptypeSymNode* proctype, const std::list<const variable*>& args);

	/*
	* Defines the never claim of the execution.
	* Set its initial FSM node in the payload.
	* Does not change the payloadHash.
	*/
	//process* addNever(const neverSymNode* neverSym);

	bool nullstate(void) const override;

	bool endstate(void) const override;

	bool isAccepting(void) const override;

	bool isAtomic(void) const;

	bool safetyPropertyViolation(void) const override;
	
	std::list<transition*> transitions(void) const override;

	std::list<transition*> executables(void) const override;

	std::list<transition*> executables(process* proc) const;

	std::list<transition*> executables(byte pid) const;

	std::map<process*, std::list<transition*>> executablesMap(void) const;

	std::list<transition*> executablesNever(void) const;

	// Applying statements

	state* apply(transition* trans) override;
	
	process* getProc(int pid) const; // Returns the stateMask with pid 'pid'.

	std::list<process*> getProcs(void) const;

	state* getNeverClaim(void) const override;

	const process* getExclusiveProc(void) const;

	byte getExclusiveProcId(void) const;

	bool hasExclusivity(void) const;

	void resetExclusivity(void) const;

	void setExclusivity(const process* proc) const;

	void setExclusivity(byte pid) const;

	//void initSym(unsigned int preOffset, const varSymNode* sym);

	//void initSymTab(unsigned int preOffset, const symTable* symTab);

	//void initGlobalVariables(void);

	//void initVariables(const process* mask);

	bool requestHandShake(const std::pair<const channel*, const process*>& handShake) const;

	void setHandShake(const std::pair<const channel*, const process*>& handShake) const;

	//void setHandShake(unsigned int cid) const;

	std::pair<const channel*, const process*> getHandShakeRequest(void) const;

	const channel* getHandShakeRequestChan(void) const;

	const process* getHandShakeRequestProc(void) const;

	unsigned int getHandShakeRequestId(void) const;

	bool hasHandShakeRequest(void) const;

	void resetHandShake(void) const;

	bool getTimeoutStatus(void) const;

	/*
	* If the pid of the last process is 'pid' then:
	*  - the stateMask of the process is removed
	*  - the number of processes in the states updated
	*  - the chunk of memory of the process is removed from the state's payload.
	* Does not change the payloadHash.
	*/
	//void killProctype(int pid);

	//void clean(void); // Applies stateKillProctype while this latter function succeeds.

	// State printing
	//void print(const state* diffState) const;

	void print(void) const override;

	//void printGraphViz(unsigned long i) const override;

	void accept(stateVisitor* visitor) override;

public:
	const symTable* const globalSymTab;
	const fsm* const stateMachine;

	unsigned int pidCounter;
	int nbProcesses; 			// Number of running processes.
	int nbNeverClaim;			// Number of neverClaim
	int lastStepPid; 			// pid of the process that fired transition that got us into this state. (NOT part of the actual state of the system, just a helper)

	mutable const channel* handShakeChan;
	mutable const process* handShakeProc;
	mutable const process* exclusiveProc;
	mutable bool timeout;

	std::list<std::string> actions;
};

#endif