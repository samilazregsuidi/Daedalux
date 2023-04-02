#include "stateToGraphViz.hpp"

#include "tvl.hpp"

stateToGraphViz::stateToGraphViz(const fsm* automata)
	: automata(automata)
	, index(0)
	, tab(1)
{}

std::string stateToGraphViz::_tab(void) const {
	std::string res = "";
	for(size_t i = 0; i < tab; ++i)
		res += "\t";
	return res;
}

stateToGraphViz::~stateToGraphViz() {}

void stateToGraphViz::printGraphViz(state* s, int depth) {
	this->depth = depth;

	file.open("trace/" + std::to_string(index++) + ".dot");

	file.precision(3);

	file << "digraph finite_state_machine {" << std::endl \
	<< _tab() << "rankdir=LR" << std::endl \
	<< _tab() << "size=\"8,5\" "<< std::endl << std::endl;

	s->accept(this);

	file << std::endl << "}" ;
	file.close();
}

void stateToGraphViz::visit(state* s) {
	assert(false);
}

void stateToGraphViz::visit(process* s) {
	auto sId = s->getVariableId();
	file << _tab() << "subgraph cluster_"<< s->getLocalName() << " {" << std::endl;
	++tab;
	file << _tab() << "style=filled;" << std::endl \
	<< _tab() << "color=white;" << std::endl \
	<< _tab() << "label = \" "<< s->getLocalName() <<" \"; " << std::endl \
	<< _tab() << sId + s->start->getID() << " [label = "<< s->start->getLineNb() <<", shape = doublecircle, "<< (s->getFsmNodePointer() == s->start ? "color = red, " : "") << "fixedsize = true]; " << std::endl \
	<< _tab() << "s" << sId + s->start->getID() << " [shape = point];" << std::endl;

	auto endEdges = s->start->getParent()->getEndTransitions(const_cast<fsmNode*>(s->start));
	if(endEdges.size() > 0)
		file << _tab() << "e" << sId + s->start->getID() << " [shape = doublecircle, fixedsize = true, style = filled, "<< (s->endstate()? "color = red, " : "fillcolor = black, fontcolor = white, ") << " label = end];\n";
	
	auto nodes = s->start->getParent()->getNodes(const_cast<fsmNode*>(s->start));
	for(auto node : nodes) {
		if(!node->getInputEdges().empty() && node != s->start)
			file << _tab() << sId + node->getID() <<" [label = "<< node->getLineNb() <<", shape = circle, "<< (s->getFsmNodePointer() == node ? "color = red, " : "") << "fixedsize = true "<< (node->isAtomic()? ", style = dotted" : "")  << "];" << std::endl;
	}

	file <<  _tab() << "s" <<  sId + s->start->getID() << " -> " << sId + s->start->getID() << ";" <<std::endl;
	
	auto edges = s->start->getParent()->getTransitions(const_cast<fsmNode*>(s->start));
	auto firedEdge = s->getOrigin() ? dynamic_cast<const processTransition*>(s->getOrigin())->getEdge() : nullptr;

	for(auto t : edges){
		auto exprStr = std::string(*t->getExpression());
		std::replace(exprStr.begin(), exprStr.end(), '\"', ' ');
		std::replace(exprStr.begin(), exprStr.end(), '\n', ' ');
		
		if(exprStr.size() > 16)
			exprStr = "...";

		if(t->getTargetNode()) {
			file << _tab() << sId + t->getSourceNode()->getID() <<" -> "<< sId + t->getTargetNode()->getID() << " [" << (firedEdge == t ? "color = red, fontcolor = red," : "") << " label = \""<< ( t->getProbability() != 1.0 ? " [" + std::to_string(t->getProbability())+"] " : "") << t->getLineNb() << " | " << (t->getFeatures()? TVL::toString(t->getFeatures()) + " | " : "") << exprStr << "\"];" << std::endl;
		}
		else
			file << _tab() << sId + t->getSourceNode()->getID() <<" -> e" << sId + s->start->getID() 		<<" [" << (firedEdge == t ? "color = red, fontcolor = red," : "") <<" label = \""<< ( t->getProbability() != 1.0 ? " [" + std::to_string(t->getProbability())+"] " : "") << t->getLineNb() << " | " << (t->getFeatures()? TVL::toString(t->getFeatures()) + " | " : "")  << exprStr << "\"];" << std::endl;
	}

	--tab;
	file << std::endl << _tab() << "}";
}

void stateToGraphViz::visit(progState* s) {
	file <<  _tab() << "subgraph cluster_"<< s->getLocalName() << " {" << std::endl;
	++tab;
	file << _tab() << "style=filled;" << std::endl \
	<<  _tab() << "color=lightgrey;" << std::endl \
	<<  _tab() << "label = \" "<< s->getLocalName() <<" \"; " << std::endl;

	for(auto subS : s->getProcs())
		subS->accept(this);


	--tab;
	file << std::endl <<  _tab() << "}" << std::endl;
}

void stateToGraphViz::visit(compState* s) {
	file <<  _tab() << "subgraph cluster_"<< s->getLocalName() << " {" << std::endl;
	++tab;
	file <<  _tab() << "style=filled;" << std::endl \
	<<  _tab() << "color=darkgrey;" << std::endl \
	<<  _tab() << "label = \" "<< s->getLocalName() <<" : depth : "<< depth <<" \"; " <<  std::endl;

	for(auto subS : s->getSubStates())
		subS->accept(this);

	--tab;
	file << std::endl <<  _tab() << "}" << std::endl;
}

void stateToGraphViz::visit(never* s) {
	file << _tab() << "subgraph cluster_"<< s->getLocalName() << " {" << std::endl;
	++tab;
	file << _tab() << "style=filled;" << std::endl \
	<< _tab() << "color=white;" << std::endl \
	<< _tab() << "label = \" "<< s->getLocalName() <<" \"; " << std::endl \
	<< _tab() << s->start->getID() << " [label = "<< s->start->getLineNb() <<", shape = doublecircle, "<< (s->getFsmNodePointer() == s->start ? "color = red, " : "") << "fixedsize = true]; " << std::endl \
	<< _tab() << "s" << s->start->getID() << " [shape = point];" << std::endl;

	auto endEdges = s->start->getParent()->getEndTransitions(const_cast<fsmNode*>(s->start));
	if(endEdges.size() > 0)
		file << _tab() << "e" << " [shape = doublecircle, fixedsize = true, style = filled, "<< (s->endstate()? "color = red, " : "fillcolor = black, fontcolor = white, ") << " label = end];\n";
	
	auto nodes = s->start->getParent()->getNodes(const_cast<fsmNode*>(s->start));
	for(auto node : nodes) {
		if(!node->getInputEdges().empty() && node != s->start)
			file << _tab() << node->getID() <<" [label = "<< node->getLineNb() <<", shape = circle, "<< (s->getFsmNodePointer() == node ? "color = red, " : "") << "fixedsize = true "<< (node->isAccepting()? ", style = filled, fillcolor = darkgrey" : "") << "];" << std::endl;
	}

	file << _tab() << "s" <<  s->start->getID() << " -> " << s->start->getID() << ";" <<std::endl;
	
	auto edges = s->start->getParent()->getTransitions(const_cast<fsmNode*>(s->start));
	auto trans = s->getOrigin();
	const neverTransition* castTrans = nullptr;
	const fsmEdge* firedEdge = nullptr;
	if(trans) {
		castTrans = dynamic_cast<const neverTransition*>(trans);
		firedEdge = castTrans->getEdge();
	}

	for(auto t : edges){
		auto exprStr = std::string(*t->getExpression());
		std::replace(exprStr.begin(), exprStr.end(), '\"', ' ');
		std::replace(exprStr.begin(), exprStr.end(), '\n', ' ');
		
		if(exprStr.size() > 16)
			exprStr = "...";

		if(t->getTargetNode()) {
			file <<_tab() <<   t->getSourceNode()->getID() <<" -> "<< t->getTargetNode()->getID() << " [" << (firedEdge == t ? "color = red, fontcolor = red," : "") <<" label = \""<< ( t->getProbability() != 1.0 ? " [" + std::to_string(t->getProbability())+"] " : "") << t->getLineNb() << " | " << exprStr << "\"];" << std::endl;
		}
		else
			file << _tab() <<  t->getSourceNode()->getID() <<" -> e" <<" [" << (firedEdge == t ? "color = red, fontcolor = red," : "") <<" label = \""<< ( t->getProbability() != 1.0 ? " [" + std::to_string(t->getProbability())+"] " : "") << t->getLineNb() << " | " << exprStr << "\"];" << std::endl;
	}

	--tab;
	file << std::endl <<  _tab() << "}" << std::endl;
}

void stateToGraphViz::visit(featStateDecorator* s) {
	s->wrappee->accept(this);
}
