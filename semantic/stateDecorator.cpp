#include "stateDecorator.hpp"
// State

stateDecorator::stateDecorator(state* wrappee) 
	: state(*wrappee)
	, wrappee(wrappee)
{
	//variable::clearVariables();
}

stateDecorator::stateDecorator(const stateDecorator& s) 
	: state(*s.wrappee)
	, wrappee(s.wrappee)
{
	//variable::clearVariables();
}

stateDecorator::stateDecorator(const stateDecorator* other)
	: state(*other->wrappee)
	, wrappee(nullptr)
{
	wrappee = other->wrappee->deepCopy();
	
	//variable::clearVariables();
}

stateDecorator::~stateDecorator() {
	
	variable::clearVariables();

	wrappee->setParent(nullptr);
	wrappee->origin = nullptr;
	delete wrappee;
}

variable::Type stateDecorator::getType(void) const {
	return wrappee->getType();
}

void stateDecorator::assign(const variable* sc) {
	wrappee->assign(sc);
}

bool stateDecorator::isGlobal(void) const {
	return wrappee->isGlobal();
}

/*bool stateDecorator::operator == (const variable& other) const {
	return getValue() == other.getValue();
}

bool stateDecorator::operator != (const variable& other) const {
	return getValue() == other.getValue();
}*/

void stateDecorator::setParent(variable* parent) {
	this->parent = parent;
	wrappee->setParent(parent);
}

variable* stateDecorator::getParent(void) const {
	return wrappee->getParent();
}

std::string stateDecorator::getFullName(void) const {
	return wrappee->getFullName();
}

std::string stateDecorator::getLocalName(void) const {
	return wrappee->getLocalName();
}

unsigned int stateDecorator::getVariableId(void) const {
	return wrappee->getVariableId();
}

void stateDecorator::_addVariable(variable* var) {
	wrappee->_addVariable(var);
	//need copy for getTVariable for ex.
	varList = wrappee->varList;
	varMap = wrappee->varMap;
}

void stateDecorator::_rmVariable(const variable* var) {
	wrappee->_rmVariable(var);
	//need copy for getTVariable for ex.
	varList = wrappee->varList;
	varMap = wrappee->varMap;
}

bool stateDecorator::hasVariables(void) const {
	return wrappee->hasVariables();
}

std::list<variable*> stateDecorator::getVariables(void) const {
	return wrappee->getVariables();
}

stateDecorator::operator::std::string(void) const {
	return wrappee->operator std::string();
}

void stateDecorator::print(void) const {
	wrappee->print();
}

void stateDecorator::printTexada(void) const {
	wrappee->printTexada();
}

void stateDecorator::printHexadecimal(void) const {
	wrappee->printHexadecimal();
}

void stateDecorator::setPayload(payload* newPayLoad) {
	wrappee->setPayload(newPayLoad);
}

payload* stateDecorator::getPayload(void) const {
	return wrappee->getPayload();
}

unsigned long stateDecorator::hash(void) const {
	return wrappee->hash();
}

size_t stateDecorator::getOffset(void) const {
	return wrappee->getOffset();
}

size_t stateDecorator::getEndOffset(void) const {
	return wrappee->getEndOffset();
}

void stateDecorator::addRawBytes(size_t size) {
	wrappee->addRawBytes(size);
}

variable* stateDecorator::getVariable(const std::string& name) const {
	return wrappee->getVariable(name);
}

channel* stateDecorator::getChannel(const std::string& name) const {
	return wrappee->getChannel(name);
}

std::map<std::string, variable*> stateDecorator::getVariablesMap(void) const {
	return wrappee->getVariablesMap();
}

size_t stateDecorator::getSizeOf(void) const {
	return wrappee->getSizeOf();
}

void stateDecorator::clearVariables(void) {
	return wrappee->clearVariables();
}

void stateDecorator::init(void) {
	wrappee->init();
}

std::list<transition*> stateDecorator::executables(void) const {
	return wrappee->executables();
}

state* stateDecorator::apply(transition* trans) {
	return wrappee->apply(trans);
}

bool stateDecorator::nullstate(void) const {
	return wrappee->nullstate();
}

bool stateDecorator::endstate(void) const {
	return wrappee->endstate();
}

bool stateDecorator::isAccepting(void) const {
	return wrappee->isAccepting();
}

bool stateDecorator::safetyPropertyViolation(void) const {
	return wrappee->safetyPropertyViolation();
}

state* stateDecorator::getNeverClaim(void) const {
	return wrappee->getNeverClaim();
}

const transition* stateDecorator::getOrigin(void) const {
	return wrappee->getOrigin();
}

double stateDecorator::getProbability(void) const {
	return wrappee->getProbability();
}

byte stateDecorator::compare(const state& s2) const {
	return wrappee->compare(s2);
}

std::list<transition*> stateDecorator::transitions(void) const {
	return wrappee->transitions();
}

void stateDecorator::accept(stateVisitor* visitor) {
	wrappee->accept(visitor);
}