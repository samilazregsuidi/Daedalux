#ifndef ENUM_VARIABLE_H
#define ENUM_VARIABLE_H

#include "scalarVar.hpp"
#include "enumDef.hpp"

template<typename T, typename variable::Type E> class enumVar : public scalar<T, E> {
public:
	enumVar(const std::string& name, T initValue, const enumDef<const T>* def)
  		: scalar<T, E>(name, initValue) 
  		, def(def)
	{}

	enumVar<T, E>* deepCopy(void) const override { return new enumVar<T, E>(*this); }

	~enumVar() override {}

	void setEnumDef(const enumDef<const T>* def) { this->def = def; }

	bool operator==(const std::string & enumName) const { return def->getEnumValue(enumName) == scalar<T, E>::getValue(); }

	bool operator!=(const std::string & cmtype) const { return !(*this == cmtype); }

	float delta(const variable * other, bool considerInternalVariables) const override
	{
		auto otherVar = dynamic_cast<const enumVar<T, E>*>(other);
		if (!otherVar) {
			// Not the same type
			return 1;
		}

		auto valueName = getValueName();
		auto otherValueName = otherVar->getValueName();
		auto hasSameValueName = (valueName.compare(otherValueName) == 0);
		if (hasSameValueName) {
			// Hack for the case where the same name is used for different values
			return 0;
		} else {
			return 1;
		}
	}

	std::string getValueName(void) const { assert(def); return def->getEnumName(this->getValue()); }

	void printDelta(const variable * other, bool considerInternalVariables) const override
	{
		if (this->isSame(other, considerInternalVariables))
			return;

		auto otherVar = dynamic_cast<const enumVar *>(other);
		if (!otherVar)
			return;

		auto name = variable::getFullName();
		auto valueName = this->getValueName();
		auto otherValueName = otherVar->getValueName();
		printf("%s: %s -> %s\n", name.c_str(), valueName.c_str(), otherValueName.c_str());
	}

	std::list<variable *> getDelta(const variable * other, bool considerInternalVariables) const override
	{
		auto otherVar = dynamic_cast<const enumVar *>(other);
		if (!otherVar || this->isSame(other, considerInternalVariables))
			return std::list<variable *>();

		std::list<variable *> res;
		res.push_back(deepCopy());
		return res;
	}

	operator std::string(void) const override
	{
		char buffer[128];
		auto value = scalar<T, E>::getValue();
		if (value) {
			auto valueName = this->getValueName();
			sprintf(buffer, "%-23s = %s\n", variable::getFullName().c_str(), valueName.c_str());
		}
		else {
			sprintf(buffer, "%-23s = nil\n", variable::getFullName().c_str());
		}
		return buffer;
	}

	void print(void) const override { printf("%s", std::string(*this).c_str()); }

	void printTexada(void) const override
	{
		if (variable::isPredef())
			return;
		auto value = scalar<T, E>::getValue();
		if (value) {
			auto valueName = this->getValueName();
			printf("%s = %s\n", variable::getFullName().c_str(), valueName.c_str());
		}
		else {
			printf("%s = nil\n", variable::getFullName().c_str());
		}
	}

	void printCSV(std::ostream & out) const override
	{
		if (variable::isPredef())
			return;
		auto value = scalar<T, E>::getValue();
		if (value) {
			auto valueName = this->getValueName();
			out << variable::getFullName() + " = " + valueName << std::endl;
		}
		else {
			out << variable::getFullName() + " = nil" << std::endl;
		}
	}

	void printCSVHeader(std::ostream & out) const override
	{
		if (variable::isPredef())
			return;
		auto value = scalar<T, E>::getValue();
		if (value) {
			auto valueName = this->getValueName();
			out << variable::getFullName() + " = " + valueName << std::endl;
		}
		else {
			out << variable::getFullName() + " = nil" << std::endl;
		}
	}

private:
	const enumDef<const T>* def;
};

#endif