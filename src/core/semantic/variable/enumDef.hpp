#ifndef ENUM_DEFINITION_H
#define ENUM_DEFINITION_H

#include <string>
#include <cassert>
#include <unordered_map>
#include <algorithm>

template <typename T> class enumDef {
public:

	enumDef(const std::string& name, const std::unordered_map<std::string, T>& values = {})
		: name(name)
		, values(values)
	{}

	const std::string& getName(void) const {
		return name;
	}

	T getEnumValue(const std::string& value) const {
		return values.at(value);
	}

	std::string getEnumName(T value) const {
		if(value == 0) {
			return "";
		}
		auto it = std::find_if(values.begin(), values.end(), [value](const std::pair<std::string, T>& p) { return p.second == value; });
		assert(it != values.end());
		return it->first;
	}

	std::unordered_map<std::string, T> getEnumMap(void) const {
		return values;
	}

	void setEnumMap(const std::unordered_map<std::string, T>& values) {
		this->values = values;
	}


private:
	std::string name;
	std::unordered_map<std::string, T> values;
};

#endif