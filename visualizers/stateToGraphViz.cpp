#include "stateToGraphViz.hpp"

stateToGraphViz::stateToGraphViz(const fsm* automata)
	: automata(automata)
	, index(0)
{}

stateToGraphViz::~stateToGraphViz() {}

void stateToGraphViz::printGraphViz(state* s) {
	file.open("trace/" + std::to_string(index) + ".dot");

	file.precision(3);

	file << "digraph finite_state_machine {" << std::endl \
	<< "\trankdir=LR" << std::endl \
	<< "\tsize=\"8,5\" "<< std::endl;

	s->accept(this);

	file << "}" ;
}

void stateToGraphViz::visit(state* s) {
	assert(false);
}

void stateToGraphViz::visit(process* s) {
	file << "\tsubgraph "<< s->getLocalName() << "{" << std::endl \
	<< "\t\tstyle=filled;" << std::endl \
	<< "\t\tcolor=white;" << std::endl \
	<< "label = \" "<< s->getLocalName() <<" \"; " << std::endl \
	<< "\t" << s->start->getID() << " [label = "<< s->start->getLineNb() <<", shape = doublecircle, "<< (s->getFsmNodePointer() == s->start ? "color = red, " : "") << "fixedsize = true]; " << std::endl \
	<< "\ts" << s->start->getID() << " [shape = point];" << std::endl;

	auto endEdges = s->start->getParent()->getEndTransitions(const_cast<fsmNode*>(s->start));
	if(endEdges.size() > 0)
		file << "\te" << s->start->getID() << " [shape = doublecircle, fixedsize = true, style = filled, "<< (s->endstate()? "color = red, " : "fillcolor = black, fontcolor = white, ") << " label = end];\n";
	
	auto nodes = s->start->getParent()->getNodes(const_cast<fsmNode*>(s->start));
	for(auto node : nodes) {
		if(!node->getInputEdges().empty())
			file << "\t "<< node->getID() <<" [label = "<< node->getLineNb() <<", shape = circle, "<< (s->getFsmNodePointer() == node ? "color = red, " : "") << "fixedsize = true "<< ((node->getFlags() & fsmNode::N_ATOMIC)? ", style = dotted" : "") << "];\n";
	}

	file << "\ts" <<  s->start->getID() << " -> " << s->start->getID() << ";" <<std::endl;
	
	auto edges = s->start->getParent()->getTransitions(const_cast<fsmNode*>(s->start));
	auto firedEdge = dynamic_cast<processTransition*>(s->getOrigin())->
	for(auto t : edges){
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

void stateToGraphViz::visit(progState* s) {
	file << "\tsubgraph "<< s->getLocalName() << "{" << std::endl \
	<< "\t\tstyle=filled;" << std::endl \
	<< "\t\tcolor=lightgrey;" << std::endl \
	<< "label = \" "<< s->getLocalName() <<" \"; " << std::endl;

	for(auto subS : s->getProcs())
		subS->accept(this);


	file << "}" << std::endl;
}

void stateToGraphViz::visit(compState* s) {
	file << "\tsubgraph "<< s->getLocalName() << "{" << std::endl \
	<< "\t\tstyle=filled;" << std::endl \
	<< "\t\tcolor=white;" << std::endl \
	<< "label = \" "<< s->getLocalName() <<" \"; " << std::endl;

	for(auto subS : s->getSubStates())
		subS->accept(this);


	file << "}" << std::endl;
}

void stateToGraphViz::visit(never* s) {

}

void stateToGraphViz::visit(featStateDecorator* s) {
	assert(false);
}
