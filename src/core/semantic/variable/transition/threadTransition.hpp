#ifndef THREAD_TRANSITION_H
#define THREAD_TRANSITION_H

/*
 * Execution of FSMs
 * * * * * * * * * * * * * * * * * * * * * * * */

#include "transition.hpp"

#include "astNode.hpp"

class fsmEdge;
class thread;

// ProcessTransitions are returned by the executables() function
class threadTransition : public transition {
public:
	threadTransition(thread* proc, const fsmEdge* trans);

	threadTransition(const threadTransition* other);

	~threadTransition() override;
	
	thread* getThread(void) const;

	const fsmEdge* getEdge(void) const;

	unsigned int getLine(void) const;

	int getLineNb(void) const;

	astNode::Type getType(void) const;

	transition* deepCopy(void) const override;

	void accept(transitionVisitor* visitor) override;

	bool operator==(const transition* other) const override;

	float similarity(const transition* other) const override;

	void print(void) const override;

public:		//
	const fsmEdge* const edge;			//  - The transition that can be fired
};

#endif
