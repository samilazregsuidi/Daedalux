#include "structVar.hpp"

structVar::structVar(const std::string& name) 
	: variable(V_STRUCT, name)
{}

structVar* structVar::deepCopy(void) const {
	return new structVar(*this);
}

float structVar::delta(const variable * v2, bool considerInternalVariables) const
{
	auto casted = dynamic_cast<const structVar *>(v2);
	if (!casted)
		return 1;

	float res = 0;
	for (auto var : varList){
		auto localName = var->getLocalName();
		auto var2 = v2->get(localName);
		res += var->delta(var2, considerInternalVariables);
	}

	return res / varList.size();
}

std::vector<std::shared_ptr<statePredicate>> structVar::getPredicates(void) const
{
	std::vector<std::shared_ptr<statePredicate>> predicates;
	for (auto var : varList){
		auto preds = var->getPredicates();
		predicates.insert(predicates.end(), preds.begin(), preds.end());
	}
	return predicates;
}


void structVar::printDelta(const variable * v2, bool considerInternalVariables) const
{
	for (auto var : varList)
		var->printDelta(v2->get(var->getLocalName()), considerInternalVariables);
}

std::list<variable *> structVar::getDelta(const variable * v2, bool considerInternalVariables) const
{
	std::list<variable *> res;
	for (auto var : varList) {
		auto v = v2->get(var->getLocalName());
		auto delta = var->delta(v, considerInternalVariables);
		if (delta > 0)
		res.push_back(v);
	}
	return res;
}
