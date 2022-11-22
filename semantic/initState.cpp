#include "initState.hpp"

#include "fsm.hpp"

#include "state.hpp"
#include "programState.hpp"
#include "compositeState.hpp"

state* initState::createInitState(const fsm* automata) {
	state* res = nullptr;

	auto sysTable = automata->getSystemSymTab();
	if(sysTable && sysTable->getSymbols().size() > 1) {
		auto compS = new compState(automata);
		for(auto sys : sysTable->getSymbols()) {
			assert(sys->getType() == symbol::T_SYS);
			compS->addState(new progState(automata, sys->getName()));
		}
		res = compS;
	} else {
		res = new progState(automata, "");
	}

	res->init();
	return res;
}