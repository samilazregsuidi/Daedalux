#pragma once

#include "formula.hpp"
#include "state.hpp"
#include "trace.hpp"
#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

// Enum class for the combination operator type
enum class CombinationOperatorType {
  AND_Symbol,
  OR_Symbol,
};

// Define the type for the predicate function
using Predicate = std::function<bool(state *)>;

// Custom comparator for the ValueType variant
using ValueType = std::variant<int, std::string, bool>;

struct KeyHash {
  std::size_t operator()(const ValueType & key) const
  {
    if (std::holds_alternative<int>(key)) {
      int value = std::get<int>(key);
      return std::hash<int>{}(value);
    }
    else if (std::holds_alternative<std::string>(key)) {
      std::string value = std::get<std::string>(key);
      return std::hash<std::string>{}(value);
    }
    else { // bool case
      bool value = std::get<bool>(key);
      return std::hash<bool>{}(value);
    }
  }
};

using ValueStateMap = std::unordered_map<ValueType, std::vector<std::shared_ptr<state>>, KeyHash>;

struct VariableUpdate {
  std::string variable_name;
  ValueType previous_value;
  ValueType new_value;

  VariableUpdate(const std::string & variable_name, const ValueType & previous_value, const ValueType & new_value)
      : variable_name(variable_name), previous_value(previous_value), new_value(new_value)
  {
    assert(previous_value.index() == new_value.index());
    assert(new_value != previous_value);
  }
};

// Struct to store the result of the values_overlap function
struct overlapResult {
  // Specifies if the value ranges overlap between the include and exclude states.
  bool value_ranges_overlap;
  // The smallest/largest value in the the exclude states, if the value ranges do not overlap.
  int boundary_value;
  // Specifies if the boundary value is larger or smaller than the value in the exclude states.
  bool is_larger;
};

template <typename T>
concept SharedPointerToFormula = std::is_same_v<T, std::shared_ptr<formula>>;

template <typename C>
concept ContainerOfFormulas = requires(C c) {
  {
    c.begin()
  } -> std::same_as<typename C::iterator>;
  {
    c.end()
  } -> std::same_as<typename C::iterator>;
  {
    c.empty()
  } -> std::same_as<bool>;
  requires SharedPointerToFormula<typename C::value_type>;
};

class formulaUtility {
public:
  template <ContainerOfFormulas FormulaContainer>
  static std::shared_ptr<formula> combineFormulas(const FormulaContainer & formulas,
                                                  const CombinationOperatorType operatorSymbol)
  {
    if (formulas.empty()) {
      std::cout << "The container of formulas is empty. I am returning a true formula." << std::endl;
      return std::make_shared<BooleanConstant>(true);
    }

    auto res_formula = *formulas.begin(); // Initialize with the first element

    switch (operatorSymbol) {
    case CombinationOperatorType::AND_Symbol:
      for (auto it = std::next(formulas.begin()); it != formulas.end(); ++it) {
        auto current_formula = *it;
        if (!isBooleanConstantWithValue(current_formula, true)) {
          res_formula = std::make_shared<AndFormula>(res_formula, current_formula);
        }
      }
      break;
    case CombinationOperatorType::OR_Symbol:
      for (auto it = std::next(formulas.begin()); it != formulas.end(); ++it) {
        auto current_formula = *it;
        if (!isBooleanConstantWithValue(current_formula, false)) {
          res_formula = std::make_shared<OrFormula>(res_formula, current_formula);
        }
      }
      break;
    default:
      throw std::invalid_argument("Unknown operator symbol");
    }

    return res_formula;
  }

  /// @brief Given a list of formulas and a formula, this function returns true if the list of formulas contains the given
  /// formula.
  /// @param formulas - The list of formulas to compare with
  /// @param formula - The formula to compare with
  /// @return True if the list of formulas contains the given formula, false otherwise
  template <ContainerOfFormulas FormulaContainer>
  static bool containsFormula(const FormulaContainer & formulas, const std::shared_ptr<formula> & formula)
  {
    return std::any_of(formulas.begin(), formulas.end(), [&formula](const auto & f) { return f->isEquivalent(*formula); });
  }

  template <ContainerOfFormulas FormulaContainer> static FormulaContainer removeDuplicates(const FormulaContainer & formulas)
  {
    FormulaContainer unique_formulas;
    std::copy_if(formulas.begin(), formulas.end(), std::back_inserter(unique_formulas),
                 [&unique_formulas](const auto & f) { return !containsFormula(unique_formulas, f); });
    return unique_formulas;
  }

  static ValueType getValueOfVariable(const state* state, const std::string & name);
  static std::shared_ptr<formula> makeRangeFormula(const std::string & name, int smallestValue, int largestValue);
  static std::shared_ptr<formula> makeEqualityFormula(const std::string & name, ValueType value, bool isInequality);
  static overlapResult values_overlap(const ValueStateMap & values1, const ValueStateMap & values2);

  static std::shared_ptr<formula> createEqualityFormulas(const std::string & variableName, const ValueStateMap & include_values,
                                                         const ValueStateMap & exclude_values);

  static std::shared_ptr<formula> makeAlternativeFormula(const std::string & name, const ValueStateMap & values,
                                                         bool isInequality);

private:
  static std::map<int, std::vector<std::shared_ptr<state>>> convertToIntegerMap(const ValueStateMap & values);

  static std::pair<int, int> extremeValues(const ValueStateMap & values);

  static bool isBooleanConstantWithValue(const std::shared_ptr<formula> & formula, bool value);
};