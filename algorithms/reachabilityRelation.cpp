#include "reachabilityRelation.hpp"

#include <stdio.h>
#include <algorithm>

#include "state.hpp"
#include "transition.hpp"
#include "process.hpp"

#include "initState.hpp"

//bad coupling?

#include "semantic.hpp"

reachabilityRelation::reachabilityRelation()
	: dfsIn(DFS_OUTER)
	, tvl(nullptr)
	, nbErrors(0)
{}

reachabilityRelation::~reachabilityRelation() {
	for(auto elem : map)
		delete elem.second;
}

void reachabilityRelation::init(state* init) {
	update(init);
	compBuilder build;
	init->accept(&build);
	violations.compMap = build.compMap;
}

void reachabilityRelation::setDFS(dfs dfs) {
	dfsIn = dfs;
}

byte reachabilityRelation::getStatus(state* s) {
	auto s_Hash = s->hash();
	auto foundIt = map.find(s_Hash);
	if(foundIt != map.end()) {
		getStatusVisitor v = getStatusVisitor(foundIt->second, s, dfsIn);
		assert(v.res != STATES_S1_NEVER_VISITED);
		return v.res;

	} else {
		return STATES_S1_NEVER_VISITED;
	}
}

void reachabilityRelation::update(state* s_) {
	auto s_Hash = s_->hash();
	auto foundIt = map.find(s_Hash);
	if(foundIt != map.end()) {
		updateVisitor v = updateVisitor(foundIt->second, s_, dfsIn, tvl);

	} else {
		map[s_->hash()] = stateToRState(s_, dfsIn);
	}
}

reachabilityRelation::dfs reachabilityRelation::lastFoundIn(state* s) const {
	auto it = map.find(s->hash());
	assert(it != map.end());
	return it->second->lastFoundIn;
}

void reachabilityRelation::addTraceViolation(state* loop) {
	loop->accept(&violations);
	++nbErrors;
}

bool reachabilityRelation::isComplete(void) const {
	return violations.isViolationsComplete();
}

bool reachabilityRelation::hasErrors(void) const {
	return nbErrors > 0;
}

/*******************************************/

reachabilityRelation::updateVisitor::updateVisitor(RState* rstate, state* s, dfs dfsIn, const TVL* tvl) 
	: current(rstate)
	, dfsIn(dfsIn)
	, tvl(tvl)
{
	s->accept(this);
}

void reachabilityRelation::updateVisitor::visit(state* s) { assert(false); }
void reachabilityRelation::updateVisitor::visit(process* s) { assert(false); }
void reachabilityRelation::updateVisitor::visit(never* s) {}
void reachabilityRelation::updateVisitor::visit(progState* s) {}

// The state was visited already, but the current copy is "fresher".
// No need to insert it into the hash table, just update the feature expression

// Important: PrevS_ can only be a state that was fully explored with the features
// it has now. This is because:
//  - it has been visited already (otherwise, it wouldn't be in the hashtab)
//  - it is not a state on the current stack (otherwise, it wouldn't be fresh)
// This means, that all states that could be visited with prevS_->features have
// been visited already.  So, when we continue, we use s_->features and not
// s_->features || prevS_->features.

void reachabilityRelation::updateVisitor::visit(featStateDecorator* s) {
	
	auto feat = (dfsIn == DFS_OUTER)? &current->outerFeatures : &current->innerFeatures;

	auto comp = s->compare(current->hash, *feat);
	if(comp == STATES_DIFF || comp == STATES_SAME_S1_VISITED) {
		return;
	}

	assert(comp == STATES_SAME_S1_FRESH);
	
	/*
	printf("found a state fresher for \n");
	s->print();

	printf("feat : \n");
	tvl->printBool(*feat);
	printf("\n\n");
	*/

	auto negPrev = ~(*feat);

	/*printf("neg : \n");
	tvl->printBool(negPrev);
	printf("\n\n");*/

	*feat |= s->getFeatures();

	/*printf("new feat : \n");
	tvl->printBool(*feat);
	printf("\n\n");*/

	/*printf("s feat : \n");
	tvl->printBool(s->getFeatures());
	printf("\n\n");*/

	s->constraint(negPrev);

	/*printf("new s feat : \n");
	tvl->printBool(s->getFeatures());
	printf("\n\n");*/
}

void reachabilityRelation::updateVisitor::visit(compState* s) {
	
	auto comp = s->compare(current->hash);
	if(comp == STATES_DIFF) {
		return;
	}

	else if (comp == STATES_SAME_S1_VISITED) {
	 	
		auto save = current;
		for(auto s : s->getSubStates()) {
			current = current->getSubHtState(s->hash());
			assert(current);
			current->lastFoundIn = dfsIn;
			s->accept(this);
			current = save;
		}
	} else {
		assert(false);
	}
}

/*******************************************************/

void reachabilityRelation::compBuilder::visit(state* s) {	assert(false);}
void reachabilityRelation::compBuilder::visit(process* s) { assert(false);}
void reachabilityRelation::compBuilder::visit(progState* s) {}
void reachabilityRelation::compBuilder::visit(never* s) {}

void reachabilityRelation::compBuilder::visit(compState* s) {
	for(auto s : s->getSubStates()) {
		s->accept(this);
	}
}

void reachabilityRelation::compBuilder::visit(featStateDecorator* s) {
	component newComp;
	newComp.name = s->getLocalName();
	newComp.productToVisit = s->getDiagram();
	compMap[s->getLocalName()] = newComp;
}

/***********************************************************************/

bool reachabilityRelation::violationsVisitor::isViolationsComplete(void) const {
	for(auto comp : compMap)
		if(!comp.second.allProductsFail)
			return false;
	return true;
}

void reachabilityRelation::violationsVisitor::visit(state* s) {	assert(false);}
void reachabilityRelation::violationsVisitor::visit(process* s) { assert(false);}
void reachabilityRelation::violationsVisitor::visit(progState* s) {}
void reachabilityRelation::violationsVisitor::visit(never* s) {}

void reachabilityRelation::violationsVisitor::visit(compState* s) {
	for(auto s : s->getSubStates()) {
		s->accept(this);
	}
}

void reachabilityRelation::violationsVisitor::visit(featStateDecorator* s) {
	auto comp = compMap[s->getLocalName()];
	if(comp.allProductsFail)
		return;
	comp.productToVisit &= ~s->getFeatures();
	comp.allProductsFail = !comp.productToVisit.IsZero();
}

/******************************************************/

reachabilityRelation::getStatusVisitor::getStatusVisitor(RState* rstate, state* s, dfs dfsIn) 
	: dfsIn(dfsIn)
	, current(rstate)
{
	s->accept(this);
}

void reachabilityRelation::getStatusVisitor::visit(state* s) {	assert(false); }
void reachabilityRelation::getStatusVisitor::visit(process* s) { assert(false); }
void reachabilityRelation::getStatusVisitor::visit(never* s) { res = s->compare(current->hash); }
void reachabilityRelation::getStatusVisitor::visit(progState* s) { res = s->compare(current->hash); }

void reachabilityRelation::getStatusVisitor::visit(featStateDecorator* s) {
	
	auto feat = (dfsIn == DFS_OUTER)? &current->outerFeatures : &current->innerFeatures;

	res = s->compare(current->hash, *feat);
	if(res == STATES_DIFF || res == STATES_SAME_S1_VISITED) {
		return;
	}

	assert(res == STATES_SAME_S1_FRESH);
}

void reachabilityRelation::getStatusVisitor::visit(compState* s) {
	auto comp = s->compare(current->hash);
	if(comp == STATES_DIFF) {
		res = STATES_DIFF;
		return;
	}

	else if (comp == STATES_SAME_S1_VISITED) {
	 	
		auto save = current;
		for(auto s : s->getSubStates()) {
			current = current->getSubHtState(s->hash());
			assert(current);
			s->accept(this);
			assert(res != STATES_DIFF);
			if(res == STATES_SAME_S1_FRESH) {
				comp = STATES_SAME_S1_FRESH;
			}
			current = save;
		}
		res = comp;
		return;
	} else {
		assert(false);
	}
}

/****************************************************/

reachabilityRelation::stateToRState::stateToRState(state* s, dfs dfsIn) {
	this->dfsIn = dfsIn;
	res = new RState(s->hash(), this->dfsIn);
	s->accept(this);
}
    
void reachabilityRelation::stateToRState::visit(state* s) { assert(false); }
void reachabilityRelation::stateToRState::visit(process* s) {}
void reachabilityRelation::stateToRState::visit(progState* s) {}
void reachabilityRelation::stateToRState::visit(never* s) {}

void reachabilityRelation::stateToRState::visit(compState* s) {
	auto save = res;
	for(auto s : s->getSubStates()) {
		RState* htS = new RState(s->hash(), dfsIn);
		res->subStates.push_back(htS);
		res = htS;
		s->accept(this);
		res = save;
	}
}

void reachabilityRelation::stateToRState::visit(featStateDecorator* s) {
	if(dfsIn == DFS_OUTER) 
		res->outerFeatures = s->getFeatures();
}

reachabilityRelation::stateToRState::operator RState*(void) const {
	return res;
}