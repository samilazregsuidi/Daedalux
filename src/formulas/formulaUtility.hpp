#pragma once

#include "formula.hpp"
#include "state.hpp"
#include "trace.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

enum class CombinationOperatorType {
  AND_Symbol,
  OR_Symbol,
};

// Custom comparator for the ValueType variant
using ValueType = std::variant<int, std::string, bool>;

struct KeyCompare {
  using is_transparent = void; // Enables heterogeneous lookup

  bool operator()(const ValueType & lhs, const ValueType & rhs) const
  {
    // Check if lhs and rhs are of the same type
    if (lhs.index() == rhs.index()) {
      // Perform comparison based on the actual type
      if (std::holds_alternative<int>(lhs)) {
        return std::get<int>(lhs) < std::get<int>(rhs);
      }
      else if (std::holds_alternative<std::string>(lhs)) {
        return std::get<std::string>(lhs) < std::get<std::string>(rhs);
      }
      else { // bool case
        // For bool, false is considered less than true
        return std::get<bool>(lhs) < std::get<bool>(rhs);
      }
    }
    // Different types are ordered by their index in the variant
    return lhs.index() < rhs.index();
  }
};

struct overlapResult {
  // Specifies if the value ranges overlap between the include and exclude states.
  bool value_ranges_overlap;
  // The smallest/largest value in the the exclude states, if the value ranges do not overlap.
  int boundary_value;
  // Specifies if the boundary value is larger or smaller than the value in the exclude states.
  bool is_larger;
};

class formulaUtility {
public:
  static std::shared_ptr<formula> combineFormulas(const std::vector<std::shared_ptr<formula>> & formulas,
                                                  const CombinationOperatorType operatorSymbol);
  static std::shared_ptr<formula> makeAlternativeFormula(std::string name,
                                                         const std::map<ValueType, std::vector<std::shared_ptr<state>>> values,
                                                         bool isInequality);
  static ValueType getValueOfVariable(const std::shared_ptr<state> & state, const std::string & name);
  static std::shared_ptr<formula> makeRangeFormula(std::string name, int smallestValue, int largestValue);
  static std::shared_ptr<formula> makeEqualityFormula(std::string name, ValueType value, bool isInequality);
  static overlapResult values_overlap(const std::map<ValueType, std::vector<std::shared_ptr<state>>> & values1,
                                      const std::map<ValueType, std::vector<std::shared_ptr<state>>> & values2);

  static int largestValue(const std::map<ValueType, std::vector<std::shared_ptr<state>>> & values);
  static int smallestValue(const std::map<ValueType, std::vector<std::shared_ptr<state>>> & values);

  static std::shared_ptr<formula>
  createEqualityFormulas(const std::string & variableName,
                         const std::map<ValueType, std::vector<std::shared_ptr<state>>> & include_values,
                         const std::map<ValueType, std::vector<std::shared_ptr<state>>> & exclude_values);

private:
  static std::map<int, std::vector<std::shared_ptr<state>>>
  convertToIntegerMap(const std::map<ValueType, std::vector<std::shared_ptr<state>>> & values);

  static bool isBooleanConstantWithValue(const std::shared_ptr<formula> & formula, bool value);
};