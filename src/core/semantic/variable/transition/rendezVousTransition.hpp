#ifndef RENDEZ_VOUS_TRANSITION_H
#define RENDEZ_VOUS_TRANSITION_H

/*
 * Execution of FSMs
 * * * * * * * * * * * * * * * * * * * * * * * */

#include "transition.hpp"

// ProcessTransitions are returned by the executables() function
class rendezVousTransition : public transition {
public:
	rendezVousTransition(state* s, transition* procTrans, transition* response = nullptr);

	rendezVousTransition(const rendezVousTransition* other);

	~rendezVousTransition() override;
	
	transition* getQuestion(void) const;

	transition* getResponse(void) const;

	transition* deepCopy(void) const override;

	void accept(transitionVisitor* visitor) override;

	bool operator==(const transition* other) const override;

	float similarity(const transition* other) const override;

	void print(void) const override;

public:		//
	transition* question;
	transition* response;
};

#endif
