#include "initState.hpp"

#include "fsm.hpp"
#include "fsmEdge.hpp"

#include "state.hpp"
#include "never.hpp"
#include "process.hpp"
#include "program.hpp"
#include "featured.hpp"
#include "composite.hpp"

#include "threadTransition.hpp"
#include "rendezVousTransition.hpp"
#include "featuredTransition.hpp"
#include "rendezVousTransition.hpp"
#include "progTransition.hpp"

#include "symbols.hpp"

#include "channelVar.hpp"
#include "structVar.hpp"
#include "boolVar.hpp"
#include "mtypeVar.hpp"

#include "argExpr.hpp"
#include "constExpr.hpp"
#include "varExpr.hpp"

#include "tvl.hpp"

#include "expToADD.hpp"

state* initState::createInitState(const fsm* automata, const TVL* tvl) {

	variable::vidCounter = 0;

	auto sysTable = automata->getSystemSymTab();
	auto compS = new composite("sys");

	if(sysTable && sysTable->getSymbols().size() > 1) {
		for(auto sys : sysTable->getSymbols()) {
			assert(sys->getType() == symbol::T_SYS);
			compS->addState(initState::createProgState(automata, sys->getName(), tvl, dynamic_cast<const sysSymNode*>(sys)));
		}

	} else
		compS->addState(initState::createProgState(automata, "", tvl));

	auto globSymTab = automata->getGlobalSymTab();
	auto neverSymList = globSymTab->getSymbols<neverSymNode*>();
	if(!neverSymList.empty()) {
		assert(neverSymList.size() == 1);
		auto never = createNever(automata, *(neverSymList.cbegin()));
		compS->addNeverState(never);
	}

	compS->init();
	return compS;
}

variable* initState::createPrimitive(const std::string& name, const varSymNode* sym) {
	
	auto initValue = 0;
	auto initExpr = sym->getInitExpr();
	if (initExpr) {
		auto initExprConst = dynamic_cast<exprConst *>(initExpr);
		// init expr should be const and const only!
		assert(initExprConst);
		initValue = initExprConst->getCstValue();
	}
	
	switch(sym->getType())
	{
	case symbol::T_BOOL:
		return new boolVar(name, initValue);
	case symbol::T_BIT:
		assert(false);
	case symbol::T_BYTE:
		return new byteVar(name, initValue);
	case symbol::T_SHORT:
		return new shortVar(name, initValue);
	case symbol::T_INT:
		return new intVar(name, initValue);
	case symbol::T_UNSGN: 	// not supported yet
		return new uintVar(name, initValue);
	case symbol::T_MTYPE:
		return new mtypeVar(name, initValue);
	case symbol::T_CMTYPE:
	{
		// its a cmtype value...
		assert(initExpr->getType() == astNode::E_EXPR_VAR);

		auto sym = dynamic_cast<exprVar *>(initExpr)->getFinalSymbol();
		assert(sym && sym->getType() == symbol::T_CMTYPE);

		initValue = dynamic_cast<const cmtypeSymNode*>(sym)->getIntValue();
		return new cmtypeVar(name, initValue);
	}
	case symbol::T_UTYPE:	// Type of variable is a user type (basically, a case symbol::T_TDEF record is being used as the type): utype points to the type record
		assert(false);
	case symbol::T_CHAN:		// Channel: capacity used; children denote message fields
		assert(false);
	case symbol::T_CID:		// Channel reference; capacity and children are not used.
		return new CIDVar(name, initValue);
	case symbol::T_MTYPE_DEF:	
		assert(false);
	case symbol::T_PID:
		return new PIDVar(name, initValue);
	case symbol::T_TDEF:		// Type definition: children denote fields of type
		assert(false);
	case symbol::T_INIT:
		assert(false);
	case symbol::T_PTYPE:		// ProcType: fsm field used; bound denotes the number of initially active processes
		assert(false);
	case symbol::T_INLINE:
		assert(false);
	case symbol::T_NEVER:	// Never claim
		assert(false);
	default:
		assert(false);
	}



	return nullptr;

}

std::list<variable*> initState::addVariables(variable* v, const varSymNode* sym) {
	assert(sym);

	std::list<variable*> res;

	switch(sym->getType())
	{
	case symbol::T_NA:
		assert(false);
	case symbol::T_BOOL:
	case symbol::T_BIT:
	case symbol::T_BYTE:
	case symbol::T_SHORT:
	case symbol::T_INT:
	case symbol::T_CID:
	case symbol::T_PID:
	case symbol::T_MTYPE:
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			
			auto name = sym->getName() + (sym->getBound() > 1 ? "[" + std::to_string(i) + "]" : "");
			auto var = createPrimitive(name, sym);
			
			v->_addVariable(var);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_UNSGN: 	// not supported yet
	case symbol::T_CLOCK:	// dense time clock
	case symbol::T_MTYPE_DEF:
		assert(false);
	case symbol::T_CMTYPE:
	{
		auto var = new cmtypeVar(dynamic_cast<const cmtypeSymNode*>(sym));
		v->_addVariable(var);
		res.push_back(var);
		return res;
	}
	//case symbol::T_FEAT,
	//case symbol::T_UFEAT,
	case symbol::T_UTYPE:	// Type of variable is a user type (basically, a case symbol::T_TDEF record is being used as the type): utype points to the type record
	{
		assert(res.size() == 0);
		if(dynamic_cast<const utypeSymNode*>(sym)->getUType()->getName() == std::string("features"))
			return res;

		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto name = sym->getName() + (sym->getBound() > 1 ? "[" + std::to_string(i) + "]" : "");
			auto var = new structVar(name);

			for(auto field : dynamic_cast<const utypeSymNode*>(sym)->getUType()->getFields()) {
				addVariables(var, field);
			}

			v->_addVariable(var);
			res.push_back(var);
		}
		return res;
	}
		// "Special" types:
	case symbol::T_CHAN:		// Channel: capacity used; children denote message fields
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new channel(dynamic_cast<const chanSymNode*>(sym), i);
			v->_addVariable(var);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_TDEF:		// Type definition: children denote fields of type
	case symbol::T_INIT:
	case symbol::T_PTYPE:		// ProcType: fsm field used; bound denotes the number of initially active processes
	case symbol::T_INLINE:
	case symbol::T_NEVER:	// Never claim
		assert(false);
	default:
		assert(false);
	}
	return res;
}

process* initState::createProcess(const fsm* stateMachine, const seqSymNode* procType, byte pid, unsigned int index) {

	auto start = stateMachine->getFsmWithName(procType->getName());

	process* proc = new process(procType, start, pid, index);

	for (auto procSym : procType->getSymTable()->getSymbols<const varSymNode*>())
		addVariables(proc, procSym);

	return proc;
}

never* initState::createNever(const fsm* stateMachine, const seqSymNode* procType) {

	auto start = stateMachine->getFsmWithName(procType->getName());

	never* proc = new never(procType, start);

	proc->_addVariable(new PIDVar(new pidSymNode(0, "_pid"), 0));

	return proc;
}

state* initState::createProgState(const fsm* stateMachine, const std::string& name, const TVL* tvl, const sysSymNode* sym) {

	state* res = nullptr;

	program* s = new program(stateMachine, name);

	for (auto sym : stateMachine->getGlobalSymTab()->getSymbols<const varSymNode*>()) {
		addVariables(s, sym);
	}

	for (const auto procSym : stateMachine->getGlobalSymTab()->getSymbols<const ptypeSymNode*>()) {
		
		assert(procSym->getActiveExpr());

		for(int i = 0; i < procSym->getActiveExpr()->getCstValue(); ++i){
			auto proc = createProcess(stateMachine, procSym, 0, i);
			s->addProcess(proc);
		}
	}

	res = s;

	if(stateMachine->isFeatured()){
		res = new featured(res, stateMachine->getFeatureDiagram(), tvl);
		if(sym && sym->getInitExpr()){
			auto v = expToADD(tvl);
			sym->getInitExpr()->acceptVisitor(&v);
			auto cst = v.getFormula();
			assert(cst);
			
			auto b = dynamic_cast<featured*>(res)->constraint(cst);
			assert(b);
		}
	}

	return res;
}

transition* initState::createProcTransition(process* proc, const fsmEdge* edge) {
	transition* res = new threadTransition(proc, edge);
	if(edge->hasFeatures())
		res = new featTransition(proc, res, edge->getFeatures());
	return res;
}