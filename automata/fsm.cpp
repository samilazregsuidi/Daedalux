#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <stack>
#include <string>
#include <assert.h>
#include <iostream>

#include "fsm.hpp"
#include "fsmNode.hpp"
#include "fsmEdge.hpp"

#include "symbols.hpp"
#include "ast.hpp"

#include "tvl.hpp"

/*
 * FINITE STATE MACHINES (FSMs)
 * * * * * * * * * * * * * * * * * * * * * * * */

fsm::fsm(const symTable* globalSymTab, const ADD& fd)
	: globalSymTab(nullptr)
	, sysSymTab(nullptr)
	, fd(fd)
{
	assert(globalSymTab->getNameSpace() == "global" || globalSymTab->getNameSpace() == "system");
	this->globalSymTab = globalSymTab->getNameSpace() == "global" ? globalSymTab : globalSymTab->getSubSymTab("global");
	this->sysSymTab = globalSymTab->getNameSpace() == "system" ? globalSymTab : globalSymTab->prevSymTab();
}

/**
 * Destroys an FSM and all that's linked to it.
 */
fsm::~fsm(){
	for(fsmNode* node : nodes) delete node;
	for(fsmEdge* trans : trans) delete trans;
}

const symTable* fsm::getGlobalSymTab(void) const {
	return globalSymTab;
}

const symTable* fsm::getSystemSymTab(void) const {
	return sysSymTab;
}

/**
 * Creates a node and adds it to the node list of the fsm.
 * The node has to be manually attached to a transition.
 */

fsmNode* fsm::createFsmNode(int flags, int lineNb) {
	fsmNode* node = new fsmNode(flags, lineNb, this);
	nodes.push_back(node);
	return node;
}

/**
 * Copies and FSM node and add it to the node list of an FSM.
 */
fsmNode* fsm::copyFsmNode(const fsmNode* node) {
	return createFsmNode(node->getFlags(), node->getLineNb());
}


fsmNode* fsm::getNode(unsigned int lineNb) const {
	for(auto node : nodes)
		if(node->getLineNb() == lineNb)
			return node;
	return nullptr;
}

std::list<fsmNode *> fsm::getNodes(void) const {
	return nodes;
}

std::list<fsmNode*> fsm::getNodes(fsmNode* from) const {
	std::list<fsmNode*> reachables;
	std::stack<fsmNode*> stack;

	stack.push(from);

	while(!stack.empty()) {
		auto current = stack.top();
		stack.pop();

		if(std::find(reachables.begin(), reachables.end(), current) == reachables.end())
			reachables.push_back(current);

		for(auto e : current->getEdges()) {
			auto next = e->getTargetNode();
			if(next && std::find(reachables.begin(), reachables.end(), next) == reachables.end()) {
				stack.push(next);
			}
		}
	}

	return reachables;
}

void fsm::deleteTransition(fsmEdge* edge) {
	assert(std::find(this->trans.begin(), this->trans.end(), edge) != this->trans.end());
	
	auto source = edge->getSourceNode();
	if(source)
		source->removeTransition(edge);
	auto dest = edge->getTargetNode();
	if(dest)
		dest->removeInputTransition(edge);

	trans.remove(edge);
	delete edge;
}

void fsm::orderAcceptingTransitions(void){
	for(auto node : nodes)
		node->orderAcceptingTransitions();
}

void fsm::removeUselessTransitions(void){
	auto tcopy = trans;
	for(auto t : tcopy) {
		if(t->expression->getType() == astNode::E_STMNT_EXPR){
			auto node = dynamic_cast<const stmntExpr*>(t->expression)->getChild();
			if(node->getType() == astNode::E_EXPR_SKIP) {
				auto src = t->getSourceNode();
				auto target = t->getTargetNode();
				if(target && target->getInputEdges().size() == 1) {	
					//assert(!target || target->getInputEdges().size() == 1);
					auto t_feat = t->getFeatures();
					deleteTransition(t);
					if(target && target->getEdges().size()) { 
						for(auto nextT : target->getEdges()) {
							nextT->setFeatures(t_feat? (nextT->getFeatures()? t->getFeatures() & nextT->getFeatures() : t_feat) : nextT->getFeatures());
							nextT->setSourceNode(src);
							src->addTransition(nextT);
						}
					}
				}
			}
		}
	}
	/*auto ncopy = nodes;
	for(auto node : ncopy)
		if(node->getInputEdges().size() == 0) {
			nodes.remove(node);
			delete node;
		}*/
}

void fsm::deleteNode(fsmNode* node) {
	assert(std::find(this->nodes.begin(), this->nodes.end(), node) != this->nodes.end());
	
	for(auto t : node->getEdges())
		deleteTransition(t);
	for(auto t : node->getInputEdges())
		deleteTransition(t);
	
	nodes.remove(node);
	delete node;
}

void fsm::connect(fsmNode* begin, fsmNode* end) {
	assert(std::find(nodes.begin(), nodes.end(), begin) != nodes.end());
	assert(std::find(nodes.begin(), nodes.end(), end) != nodes.end());

	std::list<fsmEdge*> endEdges;
	std::stack<fsmNode*> beginNodes;
	beginNodes.push(begin);

	while(!beginNodes.empty()){
		
		auto n = beginNodes.top();
		beginNodes.pop();

		for(auto t : n->getEdges()){
			if(t->getTargetNode() == nullptr)
				endEdges.push_back(t);
			else
				beginNodes.push(t->getTargetNode());
		}
	}

	for(auto t : endEdges)
		t->setTargetNode(end);
}

void fsm::addInitNode(const std::string& procName, fsmNode* node) {
	inits[procName] = node;
}

std::map<std::string, fsmNode*> fsm::getInitNodes() const {
	return inits;
}

fsmNode* fsm::getFsmWithName(const std::string& name) const {
	return inits.at(name);
}

std::list<fsmEdge *> fsm::getTransitions(void) const {
	return trans;
}

std::list<fsmEdge*> fsm::getTransitions(fsmNode* from) const {
	std::list<fsmNode*> reachables;
	std::set<fsmEdge*> edges;
	std::stack<fsmNode*> stack;

	stack.push(from);

	while(!stack.empty()) {
		auto current = stack.top();
		stack.pop();

		if(std::find(reachables.begin(), reachables.end(), current) == reachables.end())
			reachables.push_back(current);

		for(auto e : current->getEdges()) {
			edges.insert(e);
			auto next = e->getTargetNode();
			if(next && std::find(reachables.begin(), reachables.end(), next) == reachables.end()) {
				stack.push(next);
			}
		}
	}

	return std::list<fsmEdge*>(edges.begin(), edges.end());
}

std::list<fsmEdge*> fsm::getEndTransitions(void) const {
	std::list<fsmEdge*> res;
	for (auto t : trans)
        if(t->getTargetNode() == nullptr)
            res.push_back(t);
	return res;
}

std::list<fsmEdge*> fsm::getEndTransitions(fsmNode* from) const {
	std::list<fsmNode*> reachables;
	std::set<fsmEdge*> endEdges;
	std::stack<fsmNode*> stack;

	stack.push(from);

	while(!stack.empty()) {
		auto current = stack.top();
		stack.pop();

		if(std::find(reachables.begin(), reachables.end(), current) == reachables.end())
			reachables.push_back(current);

		for(auto e : current->getEdges()) {
			auto next = e->getTargetNode();
			if(!next)
				endEdges.insert(e);
			else if(std::find(reachables.begin(), reachables.end(), next) == reachables.end()) 
				stack.push(next);
		}
	}

	return std::list<fsmEdge*>(endEdges.begin(), endEdges.end());
}

void fsm::addTransition(fsmEdge* edge){
	trans.push_back(edge);
}

bool fsm::isFeatured(void) const {
	return fd;
}

const ADD& fsm::getFeatureDiagram(void) const {
	return fd;
}

void fsm::printGraphVis(std::ofstream& file) const {
	file << "digraph finite_state_machine {\n";
	file << "\trankdir=LR\n";
	file << "\tsize=\"8,5\"\n";

	for(auto init : inits) {
		file << "\t" << init.second->getID() << " [label = "<< init.second->getLineNb() <<", shape = doublecircle, fixedsize = true]; \n";
		file << "\ts" << init.second->getID() << " [shape = point];\n";
	}
	
	for(auto end : getEndTransitions())
		file << "\te" << end->getSourceNode()->getID() << " [shape = doublecircle, fixedsize = true, style = filled, fillcolor = black, fontcolor = white, label = end];\n";
	
	for(auto node : nodes) {
		if(!node->getInputEdges().empty())
			file << "\t "<< node->getID() <<" [label = "<< node->getLineNb() <<", shape = circle, fixedsize = true "<< ((node->getFlags() & fsmNode::N_ATOMIC)? ", style = dotted" : "") << "];\n";
	}

	for(auto init : inits)
		file << "\ts" <<  init.second->getID() << " -> " << init.second->getID() << ";\n";
	
	for(auto t : trans){
		auto exprStr = std::string(*t->getExpression());
		std::replace(exprStr.begin(), exprStr.end(), '\"', ' ');
		std::replace(exprStr.begin(), exprStr.end(), '\n', ' ');
		if(t->getTargetNode()) {
			file << "\t" <<  t->getSourceNode()->getID() <<" -> "<< t->getTargetNode()->getID() <<" [label = \"" << ( t->getProbability() != 1.0 ? " [" + std::to_string(t->getProbability())+"] " : "") << exprStr << "\"];\n";
		}
		else
			file << "\t" <<  t->getSourceNode()->getID() <<" -> e" << t->getSourceNode()->getID() <<" [label = \"" << ( t->getProbability() != 1.0 ? " [" + std::to_string(t->getProbability())+"] " : "") << exprStr << "\"];\n";
		
	}


	file << "}";
}

void fsm::printGraphVisWithLocations(std::ofstream& file, const std::list<const fsmNode*>& locs, const std::list<const fsmEdge*>& edges) const {
	file.precision(3);

	file << "digraph finite_state_machine {\n";
	file << "\trankdir=LR\n";
	file << "\tsize=\"8,5\"\n";

	for(auto init : inits) {
		file << "\t" << init.second->getID() << " [label = "<< init.second->getLineNb() <<", shape = doublecircle, "<< (std::find(locs.begin(), locs.end(), init.second) != locs.end()? "color = red, " : "") << "fixedsize = true]; \n";
		file << "\ts" << init.second->getID() << " [shape = point];\n";
	}
	
	for(auto end : getEndTransitions())
		file << "\te" << end->getSourceNode()->getID() << " [shape = doublecircle, fixedsize = true, style = filled, "<< (std::find(locs.begin(), locs.end(), end->getSourceNode()) != locs.end()? "color = red, " : "fillcolor = black, fontcolor = white, ") << " label = end];\n";
	
	for(auto node : nodes) {
		if(!node->getInputEdges().empty())
			file << "\t "<< node->getID() <<" [label = "<< node->getLineNb() <<", shape = circle, "<< (std::find(locs.begin(), locs.end(), node) != locs.end()? "color = red, " : "") << "fixedsize = true "<< ((node->getFlags() & fsmNode::N_ATOMIC)? ", style = dotted" : "") << "];\n";
	}

	for(auto init : inits)
		file << "\ts" <<  init.second->getID() << " -> " << init.second->getID() << ";\n";
	
	for(auto t : trans){
		auto exprStr = std::string(*t->getExpression());
		std::replace(exprStr.begin(), exprStr.end(), '\"', ' ');
		std::replace(exprStr.begin(), exprStr.end(), '\n', ' ');
		if(t->getTargetNode()) {
			file << "\t" <<  t->getSourceNode()->getID() <<" -> "<< t->getTargetNode()->getID() << " [" << (std::find(edges.begin(), edges.end(), t) != edges.end()? "color = red," : "") <<" label = \""<< ( t->getProbability() != 1.0 ? " [" + std::to_string(t->getProbability())+"] " : "") << t->getLineNb() << " | " << exprStr << "\"];\n";
		}
		else
			file << "\t" <<  t->getSourceNode()->getID() <<" -> e" << t->getSourceNode()->getID() <<" [" << (std::find(edges.begin(), edges.end(), t) != edges.end()? "color = red," : "") <<" label = \""<< ( t->getProbability() != 1.0 ? " [" + std::to_string(t->getProbability())+"] " : "") << t->getLineNb() << " | " << exprStr << "\"];\n";
		
	}

	file << "}";
}

/*
void fsm::printGraphVis(std::ofstream& file) const {
	file << "digraph finite_state_machine {\n";
	file << "\trankdir=LR\n";
	file << "\tsize=\"8,5\"\n";

	for(auto init : inits) {
		file << "\t" << init.second->getID() << " [label = "<< init.second->getLineNb() <<", shape = doublecircle, fixedsize = true]; \n";
		file << "\ts" << init.second->getID() << " [shape = point];\n";
	}
	
	for(auto end : getEndTransitions())
		file << "\te" << end->getSourceNode()->getID() << " [shape = doublecircle, fixedsize = true, style = filled, fillcolor = black, fontcolor = white, label = end];\n";
	
	for(auto node : nodes) {
		if(!node->getInputTransitions().empty())
			file << "\t "<< node->getID() <<" [label = "<< node->getLineNb() <<", shape = circle, fixedsize = true "<< ((node->getFlags() & fsmNode::N_ATOMIC)? ", style = dotted" : "") << "];\n";
	}

	for(auto init : inits)
		file << "\ts" <<  init.second->getID() << " -> " << init.second->getID() << ";\n";
	
	for(auto t : trans){
		auto exprStr = std::string(*t->getExpression());
		std::replace(exprStr.begin(), exprStr.end(), '\"', ' ');
		std::replace(exprStr.begin(), exprStr.end(), '\n', ' ');
		if(t->getTargetNode()) {
			file << "\t" <<  t->getSourceNode()->getID() <<" -> "<< t->getTargetNode()->getID() <<" [label = \""<< exprStr << "\" "<< ((t->getTargetNode()->getFlags() & fsmNode::N_ATOMIC)? ", style = dotted" : "") << "];\n";
		}
		else
			file << "\t" <<  t->getSourceNode()->getID() <<" -> e" << t->getSourceNode()->getID() <<" [label = \""<< exprStr << "\"];\n";
		
	}


	file << "}";
}*/