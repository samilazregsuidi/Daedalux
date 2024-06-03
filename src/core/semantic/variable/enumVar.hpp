#ifndef ENUM_VARIABLE_H
#define ENUM_VARIABLE_H

#include "scalarVar.hpp"
#include "enumDef.hpp"

template<typename T, typename variable::Type E> class enumVar : public scalar<T, E> {
public:
	enumVar(const std::string& name, unsigned char initValue = 0, const enumDef<T>* def = nullptr)
  		: scalar<T, E>(name, initValue) 
  		, def(def)
	{}

	enumVar<T, E>* deepCopy(void) const override { return new enumVar<T, E>(*this); }

	~enumVar() override {}

	void setEnumDef(const enumDef<T>* def) { this->def = def; }

	bool operator==(const std::string & enumName) const { return def->getEnumValue(enumName) == scalar<T, E>::getValue(); }

	bool operator!=(const std::string & cmtype) const { return !(*this == cmtype); }

	float delta(const variable* other) const override 
	{
		return 1;
		/*
		auto otherVar = dynamic_cast<const enumVar *>(other);
		if (!otherVar) {
			// Not the same E
			return 1;
		}
		auto value = getValue();
		auto otherValue = otherVar->getValue();
		auto hasSameValue = (value == otherValue);
		auto valueName = getValueName();
		auto otherValueName = otherVar->getValueName();
		auto hasSameValueName = (valueName.compare(otherValueName) == 0);
		if (hasSameValueName && !hasSameValue) {
			auto def = dynamic_cast<const mtypeSymNode *>(varSym)->getMTypeDef();
			auto def_other = dynamic_cast<const mtypeSymNode *>(otherVar->varSym)->getMTypeDef();
			auto offset = def->getIntValueOffset();
			auto offset_other = def_other->getIntValueOffset();
			auto value_minus_offset = value - offset;
			auto value_minus_offset_other = otherValue - offset_other;
			hasSameValue = (value_minus_offset == value_minus_offset_other);
		}
		return (hasSameValue && hasSameValueName) ? 0 : 1;*/
	}

	std::string getValueName(void) const { return def->getEnumName(scalar<T, E>::getValue()); }

	void printDelta(const variable * other) const override
	{
		if (variable::isSame(other))
			return;

		auto otherVar = dynamic_cast<const enumVar *>(other);
		if (!otherVar)
			return;

		auto name = variable::getFullName();
		auto valueName = this->getValueName();
		auto otherValueName = otherVar->getValueName();
		printf("%s: %s -> %s\n", name.c_str(), valueName.c_str(), otherValueName.c_str());
	}

	std::list<variable *> getDelta(const variable * other) const override
	{
		std::list<variable *> res;
		if (scalar<T, E>::isSame(other))
			return res;

		auto otherVar = dynamic_cast<const enumVar *>(other);
		if (!otherVar)
			return res;

		res.push_back(deepCopy());
		return res;
	}

	operator std::string(void) const override
	{
		char buffer[128];
		auto value = scalar<T, E>::getValue();
		if (value) {
			auto valueName = this->getValueName();
			sprintf(buffer, "0x%-4lx:   %-23s = %s\n", variable::getOffset(), variable::getFullName().c_str(), valueName.c_str());
		}
		else {
			sprintf(buffer, "0x%-4lx:   %-23s = nil\n", variable::getOffset(), variable::getFullName().c_str());
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
	const enumDef<T>* def;
};

#endif