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
{
}

reachabilityRelation::~reachabilityRelation() {
	for(auto elem : stateMap)
		delete elem.second;
}

void reachabilityRelation::init(state* init) {
	update(init);
	compBuilder build;
	build.R = this;
	init->accept(&build);
}

void reachabilityRelation::setDFS(dfs dfs) {
	dfsIn = dfs;
}

byte reachabilityRelation::getStatus(state* s) {
	auto s_Hash = s->hash();
	auto foundIt = stateMap.find(s_Hash);
	if(foundIt != stateMap.end()) {
		getStatusVisitor v = getStatusVisitor(foundIt->second, s, dfsIn);
		assert(v.res != STATES_S1_NEVER_VISITED);
		return v.res;
	} else {
		s->secret = "NEVER";
		return STATES_S1_NEVER_VISITED;
	}
}

void reachabilityRelation::update(state* s_) {
	auto s_Hash = s_->hash();
	auto foundIt = stateMap.find(s_Hash);
	if(foundIt != stateMap.end()) {
		updateVisitor v = updateVisitor(this, foundIt->second, s_, dfsIn, tvl);
	} else {
		stateMap[s_Hash] = stateToRState(s_, dfsIn);
	}
}

reachabilityRelation::dfs reachabilityRelation::lastFoundIn(state* s) const {
	auto it = stateMap.find(s->hash());
	assert(it != stateMap.end());
	return it->second->lastFoundIn;
}

void reachabilityRelation::addTraceViolation(state* loop) {
	violationsVisitor violations;
	violations.R = this;
	loop->accept(&violations);
	++nbErrors;
}

bool reachabilityRelation::isComplete(void) {
	violationsVisitor violations;
	violations.R = this;
	return violations.isViolationsComplete();
}

bool reachabilityRelation::hasErrors(void) const {
	return nbErrors > 0;
}

ADD reachabilityRelation::getFailedProducts(void) const {
	assert(compMap.size() == 1);
	for(auto comp : compMap) {
		return comp.second->productFail;
	}
	assert(false);
	return ADD();
}

/*******************************************/

reachabilityRelation::updateVisitor::updateVisitor(reachabilityRelation* R, RState* rstate, state* s, dfs dfsIn, const TVL* tvl) 
	: current(rstate)
	, R(R)
	, dfsIn(dfsIn)
{
	s->accept(this);
}

void reachabilityRelation::updateVisitor::visit(state* s) { assert(false); }
void reachabilityRelation::updateVisitor::visit(process* s) { assert(false); }
void reachabilityRelation::updateVisitor::visit(never* s) {}
void reachabilityRelation::updateVisitor::visit(program* s) {}

// The state was visited already, but the current copy is "fresher".
// No need to insert it into the hash table, just update the feature expression

// Important: PrevS_ can only be a state that was fully explored with the features
// it has now. This is because:
//  - it has been visited already (otherwise, it wouldn't be in the hashtab)
//  - it is not a state on the current stack (otherwise, it wouldn't be fresh)
// This means, that all states that could be visited with prevS_->features have
// been visited already.  So, when we continue, we use s_->features and not
// s_->features || prevS_->features.

void reachabilityRelation::updateVisitor::visit(featured* s) {
	
	auto feat = (dfsIn == DFS_OUTER)? &current->outerFeatures : &current->innerFeatures;

	auto comp = s->compare(current->hash, *feat);
	if(comp == STATES_DIFF || comp == STATES_SAME_S1_VISITED) {
		return;
	}

	/*nameComp = s->getLocalName();
	assert(R->compMap.find(nameComp) != R->compMap.end());
	auto cmp = R->compMap.find(nameComp)->second;
	if((cmp->productToVisit & s->getFeatures()).IsZero()) {
		comp = STATES_SAME_S1_VISITED;
		return;
	}*/

	assert(comp == STATES_SAME_S1_FRESH);
	
	
	/*printf("found a state fresher for \n");
	s->print();*/

	/*printf("Rfeat : \n");
	TVL::printBool(*feat);
	printf("\n\n");*/

	/*printf("sFeat : \n");
	TVL::printBool(s->getFeatures());
	printf("\n\n");*/
	

	auto negPrev = ~(*feat);

	/*printf("negRFeat : \n");
	TVL::printBool(negPrev);
	printf("\n\n");*/

	s->R = *feat;

	*feat |= s->getFeatures();

	/*printf("newRFeat : \n");
	TVL::printBool(*feat);
	printf("\n\n");*/

	s->constraint(negPrev);

	/*printf("newSFeat : \n");
	TVL::printBool(s->getFeatures());
	printf("\n\n");*/

	assert(!feat->IsZero());
}

void reachabilityRelation::updateVisitor::visit(composite* s) {
	auto comp = s->compare(current->hash);
	if(comp == STATES_DIFF) {
		return;
	}
	else if (comp == STATES_SAME_S1_VISITED) {
		current->lastFoundIn = dfsIn;

		auto save = current;
		for(auto s : s->getSubStates()) {
			current = current->getSubHtState(s);
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
void reachabilityRelation::compBuilder::visit(program* s) {}
void reachabilityRelation::compBuilder::visit(never* s) {}

void reachabilityRelation::compBuilder::visit(composite* s) {
	for(auto s : s->getSubStates()) {
		s->accept(this);
	}
}

void reachabilityRelation::compBuilder::visit(featured* s) {
	component* newComp = new component();
	newComp->name = s->getLocalName();
	newComp->productToVisit = s->getDiagram();
	newComp->productFail = TVL::getMgr()->addZero();
	newComp->allProductsFail = false;
	R->compMap[s->getLocalName()] = newComp;
}

/***********************************************************************/

bool reachabilityRelation::violationsVisitor::isViolationsComplete(void) const {
	for(auto comp : R->compMap)
		if(!comp.second->allProductsFail)
			return false;
	return true;
}

void reachabilityRelation::violationsVisitor::visit(state* s) {	assert(false);}
void reachabilityRelation::violationsVisitor::visit(process* s) { assert(false);}
void reachabilityRelation::violationsVisitor::visit(program* s) {}
void reachabilityRelation::violationsVisitor::visit(never* s) {}

void reachabilityRelation::violationsVisitor::visit(composite* s) {
	for(auto s : s->getSubStates()) {
		s->accept(this);
	}
}

void reachabilityRelation::violationsVisitor::visit(featured* s) {
	auto comp = R->compMap[s->getLocalName()];
	if(comp->allProductsFail)
		return;
	comp->productToVisit &= ~s->getFeatures();
	
	//TVL::printBool(comp->productFail);	

	assert(s->getFeatures() != TVL::getMgr()->addOne());
	
	comp->productFail |= s->getFeatures();
	
	//TVL::printBool(comp->productFail);	

	comp->allProductsFail = comp->productToVisit.IsZero();
	assert(!comp->allProductsFail || (s->getDiagram() & ~comp->productFail).IsZero());
}

/******************************************************/

reachabilityRelation::getStatusVisitor::getStatusVisitor(RState* rstate, state* s, dfs dfsIn) 
	: current(rstate)
	, dfsIn(dfsIn)
{
	s->accept(this);
}

void reachabilityRelation::getStatusVisitor::visit(state* s) {	assert(false); }
void reachabilityRelation::getStatusVisitor::visit(process* s) { assert(false); }
void reachabilityRelation::getStatusVisitor::visit(never* s) { res = s->compare(current->hash); }
void reachabilityRelation::getStatusVisitor::visit(program* s) { res = s->compare(current->hash); }

void reachabilityRelation::getStatusVisitor::visit(featured* s) {
	auto feat = (dfsIn == DFS_OUTER)? &current->outerFeatures : &current->innerFeatures;

	res = s->compare(current->hash, *feat);
	if(res == STATES_DIFF) {
		s->secret = "DIFF";
		return;
	} else if (res == STATES_SAME_S1_VISITED) {
		s->secret = "VISITED";
		s->R = *feat;
		return;
	}

	assert(res == STATES_SAME_S1_FRESH);
	s->secret = "FRESH";
	s->R = *feat;
}

void reachabilityRelation::getStatusVisitor::visit(composite* s) {
	auto comp = s->compare(current->hash);
	if(comp == STATES_DIFF) {
		res = STATES_DIFF;
		s->secret = "DIFF";
		return;
	}
	else if (comp == STATES_SAME_S1_VISITED) {
		auto save = current;
		for(auto s : s->getSubStates()) {
			current = current->getSubHtState(s);
			assert(current);
			s->accept(this);
			assert(res != STATES_DIFF);
			if(res == STATES_SAME_S1_FRESH) {
				comp = STATES_SAME_S1_FRESH;
			}
			current = save;
		}
		res = comp;
		s->secret = (res == STATES_SAME_S1_FRESH? "FRESH" : "VISITED");
		return;
	} else {
		assert(false);
	}
}

/****************************************************/

reachabilityRelation::stateToRState::stateToRState(state* s, dfs dfsIn) {
	this->dfsIn = dfsIn;
	res = new RState(s, this->dfsIn);
	s->accept(this);
}
    
void reachabilityRelation::stateToRState::visit(state* s) { 
	throw std::runtime_error("stateToRState::visit(state* s) should never be called");
 }
void reachabilityRelation::stateToRState::visit(process* s) {}
void reachabilityRelation::stateToRState::visit(program* s) {}
void reachabilityRelation::stateToRState::visit(never* s) {}

void reachabilityRelation::stateToRState::visit(composite* s) {
	auto save = res;
	for(auto s : s->getSubStates()) {
		RState* htS = new RState(s, dfsIn);
		res->subStates.push_back(htS);
		res = htS;
		s->accept(this);
		res = save;
	}
}

void reachabilityRelation::stateToRState::visit(featured* s) {
	if(dfsIn == DFS_OUTER) {
		assert(s->getFeatures());
		res->outerFeatures = s->getFeatures();
		res->innerFeatures = TVL::getMgr()->addZero();
	} else {
		throw std::runtime_error("stateToRState::visit(featured* s) should only be called during outer DFS");
	}
}

reachabilityRelation::stateToRState::operator RState*(void) const {
	return res;
}