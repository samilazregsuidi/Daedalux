#include <daedalux/promela/semantic/variable/state/initState.hpp>

#include <daedalux/core/automata/fsm.hpp>
#include <daedalux/core/automata/fsmEdge.hpp>

#include <daedalux/core/semantic/variable/state/stateVar.hpp>
#include <daedalux/promela/semantic/variable/state/never.hpp>
#include <daedalux/promela/semantic/variable/state/process.hpp>
#include <daedalux/promela/semantic/variable/state/program.hpp>
#include <daedalux/feature/semantic/variable/state/featured.hpp>
#include <daedalux/core/semantic/variable/state/composite.hpp>

#include <daedalux/core/semantic/variable/transition/threadTransition.hpp>
#include <daedalux/core/semantic/variable/transition/rendezVousTransition.hpp>
#include <daedalux/feature/semantic/variable/transition/featuredTransition.hpp>
#include <daedalux/promela/semantic/variable/transition/progTransition.hpp>

#include <daedalux/promela/symbol.hpp>

#include <daedalux/promela/semantic/variable/channel.hpp>
#include <daedalux/promela/semantic/variable/utypeVariable.hpp>
#include <daedalux/promela/semantic/variable/boolVariable.hpp>
#include <daedalux/promela/semantic/variable/mtypeVariable.hpp>

#include <daedalux/promela/ast/expr/argExpr.hpp>
#include <daedalux/promela/ast/expr/constExpr.hpp>
#include <daedalux/promela/ast/expr/varExpr.hpp>

#include <daedalux/feature/tvl.hpp>

#include <daedalux/feature/expToADD.hpp>




#include <daedalux/core/semantic.hpp>

state* initState::createInitState(const fsm* automata, const TVL* tvl) {
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

std::list<variable*> initState::addVariables(variable* v, const varSymNode* sym) {
	assert(sym);

	std::list<variable*> res;

	switch(sym->getType())
	{
	case symbol::T_NA:
		assert(false);
	case symbol::T_BOOL:
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new boolVar(dynamic_cast<const boolSymNode*>(sym), i);
			v->_addVariable(var);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_BIT:
	case symbol::T_BYTE:
	case symbol::T_SHORT:
	case symbol::T_INT:
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new primitiveVariable(sym, i);
			v->_addVariable(var);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_UNSGN: 	// not supported yet
		assert(false);
	case symbol::T_MTYPE:
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new mtypeVar(dynamic_cast<const mtypeSymNode*>(sym), i);
			v->_addVariable(var);
			res.push_back(var);
		}
		return res;
	}
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
			auto var = new utypeVar(dynamic_cast<const utypeSymNode*>(sym), i);

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
	case symbol::T_CID:		// Channel reference; capacity and children are not used.
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new CIDVar(dynamic_cast<const cidSymNode*>(sym), i);
			v->_addVariable(var);
			res.push_back(var);
		}
		return res;
	}
	case symbol::T_PID:
	{
		for(unsigned int i = 0; i < sym->getBound(); ++i) {
			auto var = new PIDVar(dynamic_cast<const pidSymNode*>(sym), i);
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

transition* initState::createTransition(const fsmEdge* edge, program* s, process* proc, transition* response) {
	
	transition* res = new threadTransition(proc, edge);

	if(response)
		res = new rendezVousTransition(s, res, response);	
	else
		res = new programTransition(s, dynamic_cast<threadTransition*>(res));

	if(edge->hasFeatures())
		res = new featTransition(s, res, edge->getFeatures());
	
	return res;
}
