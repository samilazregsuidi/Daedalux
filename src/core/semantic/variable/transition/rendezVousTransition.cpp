#include "rendezVousTransition.hpp"
#include "transitionVisitor.hpp"

#include "fsmEdge.hpp"
#include "process.hpp"

#include <assert.h>
#include <iterator>
#include <iostream>

rendezVousTransition::rendezVousTransition(state* s, transition* question, transition* response) 
	: transition(s)
	, question(question)
	, response(response)
{
	assert(s);
	assert(question);

	prob = question->getProbability() * (response ? response->getProbability() : 1.0);
	assert(prob >= 0 && prob <= 1);

	add(question);
	if(response)
		add(response);

	/*lines.push_back(dynamic_cast<processTransition*>(question)->getLineNb());
	if(response){
		auto casted = dynamic_cast<processTransition*>(response);
		if (casted)
			lines.push_back(casted->getLineNb());
	}*/

	action = question->action;

}

rendezVousTransition::rendezVousTransition(const rendezVousTransition* other)
	: transition(other)
	, question(nullptr)
	, response(nullptr)
{
	question = other->question->deepCopy();
	if(other->response)
		response = other->response->deepCopy();
}

rendezVousTransition::~rendezVousTransition() {
	delete question;
	if(response)
		delete response;
}

transition* rendezVousTransition::getQuestion(void) const {
	return question;
}

transition* rendezVousTransition::getResponse(void) const {
	return response;
}

transition* rendezVousTransition::deepCopy(void) const {
	return new rendezVousTransition(this);
}

void rendezVousTransition::accept(transitionVisitor* visitor) {
	visitor->visit(this);
}

bool rendezVousTransition::operator==(const transition* other) const {
	auto cast = dynamic_cast<const rendezVousTransition*>(other);
	return *question == cast->question && *response == cast->response;
}

//to improve
float rendezVousTransition::similarity(const transition* other) const {
	auto cast = dynamic_cast<const rendezVousTransition*>(other);
	return cast ? question->similarity(cast->question) * (response ? response->similarity(cast->response) : 1.0) : 0;
}

void rendezVousTransition::print(void) const {
	std::cout << "RendezVousTransition: " << std::endl;
	question->print();
	if(response)
		response->print();
}