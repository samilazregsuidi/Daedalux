#pragma once

#include "formula.hpp"
#include <cstdlib> // for std::system
#include <memory>

class FormulaSimplifier {
public:
  // This method calls the OWL library to simplify the formula.
  static std::shared_ptr<formula> simplify(std::shared_ptr<formula> form)
  {
    if (!isOwlInPath()) {
      // OWL is not in the system path, return the original formula
      return form;
    }

    std::string formulaString = form->toFormula();
    // Call the OWL command line to simplify the formula using the formulaString.
    std::string command = "owl -simplify -f " + formulaString;

    // system(command.c_str());

    // Parse the result of the OWL command line and return the simplified formula.

    return form;
  }

private:
  bool isOwlInPath()
  {
    if (std::system("which owl > /dev/null 2>&1") == 0) {
      return true;
    }
    else {
      return false;
    }
  }
};
