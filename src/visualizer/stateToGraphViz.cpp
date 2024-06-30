#include "stateToGraphViz.hpp"

#include "tvl.hpp"

#define PRINT_LIMIT 512
#define PRINT_SIZE_LIMIT 128

stateToGraphViz::stateToGraphViz(const fsm* automata)
	: automata(automata)
	, index(0)
	, tab(1)
	, in(NONE)
{}

std::string stateToGraphViz::_tab(void) const {
	return std::string(tab, '\t');
}

stateToGraphViz::~stateToGraphViz() {}

void stateToGraphViz::printGraphViz(state* s, int depth) {
	this->depth = depth;

	if(index > PRINT_LIMIT)
		return;

	file.open("trace/" + std::to_string(index++) + ".dot");

	if(index == 137) {
		printf("start debugging at node");
	}

	file.precision(3);

	file << "digraph finite_state_machine {" << std::endl \
	<< _tab() << "rankdir=LR" << std::endl \
	<< _tab() << "size=\"8,5\" "<< std::endl << std::endl;

	s->accept(this);

	file << std::endl << "}" ;
	file.close();
}

void stateToGraphViz::setIn(In in) {
	this->in = in;
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
	<< _tab() << "label = "<< ((s->getProgState()->getExclusiveProcId() != s->getPid())? "\"" + s->getLocalName() +"\"" : "<<B>"+ s->getLocalName() +"</B>>") <<" ; " << std::endl \
	<< _tab() << sId + s->start->getID() << " [label = "<< s->start->getLineNb() <<", shape = doublecircle, "<< (s->getFsmNodePointer() == s->start ? "color = red, " : "") << "fixedsize = true]; " << std::endl \
	<< _tab() << "s" << sId + s->start->getID() << " [shape = point];" << std::endl;

	file << " \"node" << s->getVariableId() << "\"[ " << std::endl \
	<< _tab() << "label = \"";
	auto primVars = s->getAll<scalarInt*>();
	for(auto primVar : primVars) {
		auto str = std::string(*primVar);
		if(str.size() && str.size() < 64 && !(primVar->isHidden() || primVar->isPredef()))
			file << str << " | ";
	}
	file << "\"" << std::endl << _tab() << "shape = \"record\" "<< std::endl << "];" << std::endl;

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
	auto firedEdge = s->getOrigin() ? dynamic_cast<const threadTransition*>(s->getOrigin())->getEdge() : nullptr;

	for(auto t : edges){
		auto exprStr = std::string(*t->getExpression());
		std::replace(exprStr.begin(), exprStr.end(), '\"', ' ');
		std::replace(exprStr.begin(), exprStr.end(), '\n', ' ');
		
		if(exprStr.size() > PRINT_SIZE_LIMIT)
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

void stateToGraphViz::visit(program* s) {
	file <<  _tab() << "subgraph cluster_"<< s->getLocalName() << " {" << std::endl;
	++tab;
	file << _tab() << "style=filled;" << std::endl \
	<<  _tab() << "color=lightgrey;" << std::endl \
	<<  _tab() << "label = \" "<< s->getLocalName() << " : " << (featToPrint? TVL::toString(featToPrint) : "") << (Rfeat? " || R :" + TVL::toString(Rfeat) : " || R : NONE") << " " << s->secret << " \"; " << std::endl;

	file << " \"node" << s->getVariableId() << "\"[ " << std::endl \
	<< _tab() << "label = \"";
	auto primVars = s->getAll<scalarInt*>();
	for(auto primVar : primVars) {
		auto str = std::string(*primVar);
		if(str.size() && str.size() < 64 /*&& !(primVar->isHidden || primVar->isPredef)*/)
			file << str << " | ";
	}
	file << "\"" << std::endl << _tab() << "shape = \"record\" "<< std::endl << "];" << std::endl;

	for(auto subS : s->getProcs())
		subS->accept(this);


	--tab;
	file << std::endl <<  _tab() << "}" << std::endl;
}

std::string stateToGraphViz::_toInStrDescr(void) const {
	std::string res;
	switch (in)
	{
	case NONE:
		return res;
	case PREFIX:
		return "PREFIX";
	case CYCLE_BEGIN:
		return "CYCLE BEGIN";
	case CYCLE_END:
		return "CYCLE_END";
	case CYCLE:
		return "CYCLE";
	default:
		assert(false);
		return res;
	}
}

void stateToGraphViz::visit(composite* s) {
	file <<  _tab() << "subgraph cluster_"<< s->getLocalName() << " {" << std::endl;
	++tab;
	file <<  _tab() << "style=filled;" << std::endl \
	<<  _tab() << "color=darkgrey;" << std::endl \
	<<  _tab() << "label = \" "<< s->getLocalName() <<" : depth : "<< depth <<" | "<< _toInStrDescr() <<" "<< s->secret << "\"; "  <<  std::endl;

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
	const threadTransition* castTrans = nullptr;
	const fsmEdge* firedEdge = nullptr;
	if(trans) {
		castTrans = dynamic_cast<const threadTransition*>(trans);
		firedEdge = castTrans->getEdge();
	}

	for(auto t : edges){
		auto exprStr = std::string(*t->getExpression());
		std::replace(exprStr.begin(), exprStr.end(), '\"', ' ');
		std::replace(exprStr.begin(), exprStr.end(), '\n', ' ');
		
		if(exprStr.size() > 0)
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

void stateToGraphViz::visit(featured* s) {
	//featToPrint = s->choices;
	featToPrint = s->features;
	Rfeat = s->R;
	//assert(s->secret != std::string(""));
	s->wrappee->secret = s->secret;
	s->wrappee->accept(this);
	featToPrint = ADD();
}
