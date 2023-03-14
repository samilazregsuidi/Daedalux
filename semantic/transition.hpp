#ifndef TRANSITION_H
#define TRANSITION_H

/*
 * Execution of FSMs
 * * * * * * * * * * * * * * * * * * * * * * * */

#include <list>

class state;

typedef char byte;
typedef unsigned char ubyte;

class transition {
public:
	static transition* sample(const std::list<transition*>& transList); // Choose a transition and returns it.
	
	static void destroyProcTransList(std::list<transition*> transList, byte process_or_direct);
	
	static void checkProcTransList(std::list<transition*> list);
	
	static byte isProbabilisticTransList(std::list<transition*> list);

	/***************************************************************************************************/

	transition(state* src);

	virtual ~transition();

	double getProbability(void) const;

	//std::list<transition*> getResponses(void) const;
	
	//virtual transition* deepCopy(void) const;

	//virtual void fire(state* s) const = 0;

public:		//
	state* src;
	double prob;
	//std::list<transition*> responses;
	std::list<unsigned int> lines;
};

#endif
