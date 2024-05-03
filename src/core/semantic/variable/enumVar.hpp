#ifndef ENUM_VARIABLE_H
#define ENUM_VARIABLE_H

#include "scalarVar.hpp"
#include "enumDef.hpp"

template<typename T, variable::Type type> class enumVar : public scalar<T, type> {
public:
	enumVar(const std::string& name, unsigned char initValue = 0, const enumDef<T>* def = nullptr)
  		: scalar<T, type>(name, initValue) 
  		, def(def)
	{}

	enumVar<T>* deepCopy(void) const override { return new enumVar<T>(*this); }

	~enumVar() override {}

	void setEnumDef(const enumDef<T>* def) { this->def = def; }

	T operator++(void) override { assert(false); }

	T operator--(void) override { assert(false); }

	T operator++(int) override { assert(false); }

	T operator--(int) override { assert(false); }

	bool operator==(const std::string & enumName) const { return def->getEnumValue(enumName) == getValue(); }

	bool operator!=(const std::string & cmtype) const { return !(*this == cmtype); }

	float delta(const variable* other) const override 
	{
		return 1;
		/*
		auto otherVar = dynamic_cast<const enumVar *>(other);
		if (!otherVar) {
			// Not the same type
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

	std::string enumVar::getValueName(void) const { return def->getEnumName(getValue()); }

	void enumVar::printDelta(const variable * other) const override
	{
		if (isSame(other))
			return;

		auto otherVar = dynamic_cast<const enumVar *>(other);
		if (!otherVar)
			return;

		auto name = getFullName();
		auto valueName = getValueName();
		auto otherValueName = otherVar->getValueName();
		printf("%s: %s -> %s\n", name.c_str(), valueName.c_str(), otherValueName.c_str());
	}

	std::list<variable *> enumVar::getDelta(const variable * other) const override
	{
		std::list<variable *> res;
		if (isSame(other))
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
		auto value = getValue();
		if (value) {
			auto valueName = getValueName();
			sprintf(buffer, "0x%-4lx:   %-23s = %s\n", getOffset(), getFullName().c_str(), valueName.c_str());
		}
		else {
			sprintf(buffer, "0x%-4lx:   %-23s = nil\n", getOffset(), getFullName().c_str());
		}
		return buffer;
	}

	void print(void) const override { printf("%s", std::string(*this).c_str()); }

	void printTexada(void) const override
	{
		if (isPredef())
			return;
		auto value = getValue();
		if (value) {
			auto valueName = getValueName();
			printf("%s = %s\n", getFullName().c_str(), valueName.c_str());
		}
		else {
			printf("%s = nil\n", getFullName().c_str());
		}
	}

	void printCSV(std::ostream & out) const override
	{
		if (isPredef())
			return;
		auto value = getValue();
		if (value) {
			auto valueName = getValueName();
			out << getFullName() + " = " + valueName << std::endl;
		}
		else {
			out << getFullName() + " = nil" << std::endl;
		}
	}

	void printCSVHeader(std::ostream & out) const override
	{
		if (isPredef())
			return;
		auto value = getValue();
		if (value) {
			auto valueName = getValueName();
			out << getFullName() + " = " + valueName << std::endl;
		}
		else {
			out << getFullName() + " = nil" << std::endl;
		}
	}

private:
	const enumDef<T>* def;
};

#endif