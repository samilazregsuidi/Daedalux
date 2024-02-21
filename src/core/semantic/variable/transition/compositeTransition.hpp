#ifndef COMPOSITE_TRANSITION_H
#define COMPOSITE_TRANSITION_H

/*
 * Execution of FSMs
 * * * * * * * * * * * * * * * * * * * * * * * */

#include <list>
#include <vector>

#include "transition.hpp"

class state;

// ProcessTransitions are returned by the executables() function
class compTransition : public transition {
public:
	compTransition(state* s, const std::list<transition*>& ts);

	compTransition(const compTransition* other);
	
	~compTransition() override;
	
	transition* deepCopy(void) const override;

	void accept(transitionVisitor* visitor) override;

	bool operator==(const transition* other) const override;

	float similarity(const transition* other) const override;

	//state* fire(void) override;

	void add(transition* t);

	void add(const std::list<transition*>& Ts);

	void detach(void);

	void detach(transition* t);

	void detach(const std::list<transition*>& Ts);

	std::list<transition*> getSubTs(void) const;

	void print(void) const override;

private:
	compTransition* parent;
	std::list<transition*> subTs; 
	std::list<unsigned int> lines;
};

#endif
