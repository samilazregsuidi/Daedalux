#ifndef TRANSITION_H
#define TRANSITION_H

/*
 * Execution of FSMs
 * * * * * * * * * * * * * * * * * * * * * * * */

#include <list>
#include <vector>
#include <string>

class state;

class transitionVisitor;

typedef char byte;
typedef unsigned char ubyte;

class transition {
public:
	static transition* sampleUniform(const std::list<transition*>& transList); // Choose a transition and returns it.

	static transition* sampleNonUniform(const std::list<transition*>& transList); // Choose a transition and returns it.

	static transition* select(const std::list<transition*>& transList, const std::string& action);
	
	static void destroyProcTransList(std::list<transition*> transList, byte process_or_direct);
	
	static void checkProcTransList(std::list<transition*> list);
	
	static byte isProbabilisticTransList(std::list<transition*> list);

	static void erase(const std::list<transition*>& list);

	/***************************************************************************************************/

	transition(state* src);

	transition(const transition* other);

	virtual ~transition();

	double getProbability(void) const;
	
	virtual transition* deepCopy(void) const;

	void add(transition* t);

	void add(const std::list<transition*>& Ts);

	void detach(void);

	void detach(transition* t);

	void detach(const std::list<transition*>& Ts);

	virtual void accept(transitionVisitor* visitor);

	virtual bool operator==(const transition* other) const;

	virtual float similarity(const transition* other) const;

	// friend bool operator== (const transition& t1, const transition& t2) {
	// 	return t1.src->compare(t2.src->hash()) && t1.dst->compare(t2.dst->hash()) && t1.prob == t2.prob && t1.action == t2.action;
	// }

	// friend bool operator!= (const transition& t1, const transition& t2) {
	// 	return !(t1 == t2);
	// }

public:
	transition* parent;
	state* src;
	state* dst;
	double prob;
	std::list<transition*> subTs; 
	std::list<unsigned int> lines;
	std::string action;
};

#endif
