#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <assert.h>
#include <iostream>

#include "fsmNode.hpp"
#include "fsmEdge.hpp"
#include "automata.hpp"

#include "symbols.hpp"
#include "ast.hpp"

/**
 * Creates a transition and adds it to the node list of the fsm.
 */
fsmEdge* fsmNode::createfsmEdge(int lineNb, const astNode* expression, fsmNode* target, bool owner) {
	fsmEdge* newTrans = new fsmEdge(this, expression, lineNb, owner);
	if(target)
		newTrans->setTargetNode(target);
	this->trans.push_back(newTrans);
	parent->addTransition(newTrans);
	return newTrans;
	
}

/**
 * Copies and FSM transition and add it to the transition list of an FSM and of the transition list of the corresponding source node in this FSM.
 */
fsmEdge* fsmNode::copyfsmEdge(const fsmEdge* trans) {
	return createfsmEdge(trans->getLineNb(), trans->getExpression());
}

fsmNode::fsmNode(int flags, int lineNb, fsm* parent) 
	: parent(parent)
	, flags(flags)
	, lineNb(lineNb)
{
	flags |= N_ATOMIC;
	//std::cout << "create Node " << lineNb << std::endl;
}

fsmNode::~fsmNode(void){
}

void fsmNode::addTransition(fsmEdge* trans) {
	//assert(std::find(this->trans.begin(), this->trans.end(), trans) == this->trans.end());
	this->trans.push_back(trans);

	//std::cout << "add (n"<< lineNb << ", e" << trans->getLineNb() << ", n" << (trans->getTargetNode() ? trans->getTargetNode()->getLineNb() : -1) << ")" << std::endl;
	assert(trans->getSourceNode() == this || trans->getSourceNode() == nullptr);
}

void fsmNode::removeTransition(fsmEdge* trans) {
	assert(std::find(this->trans.begin(), this->trans.end(), trans) != this->trans.end());
	this->trans.remove(trans);

	//std::cout << "rm (n"<< lineNb << ", e" << trans->getLineNb() << ", n" << (trans->getTargetNode() ? trans->getTargetNode()->getLineNb() : -1) << ")" << std::endl;
	assert(trans->getSourceNode() == this);
}

void fsmNode::detachTransitions(void) {
	trans.clear();
}

void fsmNode::addInputTransition(fsmEdge* trans_in) {
	assert(std::find(this->trans_in.begin(), this->trans_in.end(), trans_in) == this->trans_in.end());
	this->trans_in.push_back(trans_in);

	//std::cout << "add (n"<< trans_in->getSourceNode()->getLineNb() << ", e" << trans_in->getLineNb() << ", n" << lineNb << ")" << std::endl;
	assert(trans_in->getTargetNode() == this);
}

void fsmNode::removeInputTransition(fsmEdge* trans_in) {
	assert(std::find(this->trans_in.begin(), this->trans_in.end(), trans_in) != this->trans_in.end());
	this->trans_in.remove(trans_in);

	//std::cout << "rm (n"<< trans_in->getSourceNode()->getLineNb() << ", e" << trans_in->getLineNb() << ", n" << lineNb << ")" << std::endl;
	assert(trans_in->getTargetNode() == this);
}

std::list<fsmEdge *> fsmNode::getEdges(void) const {
	return trans;
}

std::list<fsmEdge *> fsmNode::getInputEdges(void) const {
	return trans_in;
}

void fsmNode::addFlags(unsigned int flags) {
	this->flags |= flags;
}

unsigned int fsmNode::getFlags(void) const {
	return flags;
}

bool fsmNode::isAccepting(void) const {
	return flags & N_ACCEPT;
}

bool fsmNode::isProgress(void) const {
	return flags & N_PROGRESS;
}

bool fsmNode::isDeterministic(void) const {
	return flags & N_DETERMINISTIC;
}

bool fsmNode::isAtomic(void) const {
	return flags & N_ATOMIC;
}

void fsmNode::setLineNb(int line) {
	lineNb = line;
}

int fsmNode::getLineNb(void) const {
	return lineNb;
}

fsm* fsmNode::getParent(void) const {
	return parent;
}

void fsmNode::orderAcceptingTransitions(void) {
	std::list<fsmEdge*> newTrans;
	for(auto e : trans) {
		auto targetNode = e->getTargetNode();
		if(targetNode && targetNode->isAccepting())
			newTrans.push_front(e);
		else
			newTrans.push_back(e);
	}
	trans = newTrans;
}

fsmNode::operator std::string(void) const {
	std::string res = "";
	for(auto t : trans)
		res += std::string(*t);
	return res;
}

bool fsmNode::operator==(const fsmNode& other) const {
	for(auto t : trans) {
		bool found = false;
		for(auto o : other.trans) {
			if(*t == *o) {
				found = true;
				break;
			}
		}
		if(!found)
			return false; 
	}

	for(auto t : trans_in) {
		bool found = false;
		for(auto o : other.trans_in) {
			if(*t == *o) {
				found = true;
				break;
			}
		}
		if(!found)
			return false; 
	}
	
	return true;
}


unsigned long fsmNode::getID(void) const {
	unsigned long id = (unsigned long)this;
	return id;
}
/*
void fsmNode::printFsmNode(ptList printed, int level) {
	printed = listAdd(printed, this);
	spaces(level);
	printf("NL%03d", lineNb);
	if(flags != 0) printf(", flags:%s%s%s%s.", (flags & N_ACCEPT) == N_ACCEPT ? " accept" : "", (flags & N_END) == N_END ? " end" : "", (flags & N_PROGRESS) == N_PROGRESS ? " progress" : "", (flags & N_ATOMIC) == N_ATOMIC ? " atomic" : "");
	ptList tmp = trans_in;
	printf("   [in:");
	while(tmp) {
		printf("  %03d", ((fsmEdge*)tmp->value)->lineNb);
		tmp = tmp->next;
	}
	printf("]\n");
	ptList lTrans = trans;
	fsmEdge* trans;
	while(lTrans != nullptr) {
		trans = (fsmEdge*) lTrans->value;
		spaces(level);
		printf("  ----(TL%03d, %s, %s", trans->lineNb, trans->expression ? expNode::getExpTypeName(trans->expression->type) : "no expression", trans->hasFeatures ? "" : "no feature expression");
		//printBool(trans->features);
#ifdef STOCHASTIC
		printf(", %f", trans->expression ? trans->expression->prob : 1);
#endif
#ifdef CEGARTRANS
		printf(" (originally: ");
		//printBool(trans->origFeatures);
		printf(")");
#endif
		printf(")---> ");
		if(trans->target) {
			if(!listFind(printed, trans->target)) {
				printf("\n");
				trans->target->printFsmNode(printed, level + LEVEL_STEP);
			} else {
				printf("NL%03d\n", trans->target->lineNb);
			}
		} else {
			printf("(loose end)\n");
		}
		lTrans = lTrans->next;
	}
}*/