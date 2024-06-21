#ifndef SPINRUNNER_HPP
#define SPINRUNNER_HPP

#include <string>
#include <memory>
#include "../formulas/formula.hpp"

class spinRunner {
public:
  static bool check(const std::string & input, const std::shared_ptr<formula> & f);

private:
  // static std::string exec(const char * cmd);
  static bool spinInstalledFlag;
};

#endif // SPINRUNNER_HPP