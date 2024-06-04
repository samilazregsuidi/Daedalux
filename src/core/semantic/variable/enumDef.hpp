#ifndef ENUM_DEFINITION_H
#define ENUM_DEFINITION_H

#include <string>
#include <cassert>
#include <unordered_map>

template <typename T> class enumDef {
public:

	enumDef(const std::string& name, const std::unordered_map<std::string, T>& values)
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
		for(auto& pair : values) {
			if(pair.second == value) {
				return pair.first;
			}
		}
		assert(false);
		return "";
	}


private:
	std::string name;
	std::unordered_map<std::string, T> values;
};

#endif