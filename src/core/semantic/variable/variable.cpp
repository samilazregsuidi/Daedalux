#include "variable.hpp"

#include <iterator>
#include <numeric>
#include <algorithm>

#include "payload.hpp"

#include "argExpr.hpp"
#include "constExpr.hpp"
#include "varExpr.hpp"

variable::Type variable::getVarType(symbol::Type type)
{
  switch (type) {
  case symbol::T_BIT:
    return V_BIT;
  case symbol::T_BOOL:
    return V_BOOL;
  case symbol::T_BYTE:
    return V_BYTE;
  case symbol::T_CHAN:
    return V_CHAN;
  case symbol::T_MTYPE:
    return V_MTYPE;
  case symbol::T_PID:
    return V_PID;
  case symbol::T_SHORT:
    return V_SHORT;
  case symbol::T_CID:
    return V_CID;
  case symbol::T_INT:
    return V_INT;
  case symbol::T_UTYPE:
    return V_STRUCT;
  case symbol::T_CMTYPE:
    return V_CMTYPE;
  default:
    assert(false);
    return V_NA;
  }
  assert(false);
  return V_NA;
}

unsigned int variable::vidCounter = 0;

variable::variable(Type varType, const std::string & name)
    : name(name), parent(nullptr), vid(++vidCounter), varType(varType), rawBytes(0),
      hidden(false), predef(false), global(false)
{
}

variable::variable(const variable & other)
    : name(other.name), parent(other.parent), vid(other.vid), varType(other.varType), rawBytes(other.rawBytes),
      varMap(other.varMap), varList(other.varList), hidden(other.hidden),
      predef(other.predef), global(other.global)
{
  auto nbVariables = other.getVariables().size();
  // auto otherSizeOf = other->getSizeOf();
  clearVariables();

  for (auto subVar : other.getVariables()) {
    _addVariable(subVar->deepCopy());
  }

  for (auto subVar : getVariables())
    subVar->assign(subVar);

  assert(getVariables().size() == nbVariables);
  /*auto thisSizeOf = getSizeOf();
  if(thisSizeOf != otherSizeOf)
          assert(false);
  assert(getSizeOf() == otherSizeOf);*/
}

variable::~variable()
{
  auto copy = varList;
  for (auto var : copy)
    delete var;

  assert(varList.empty());

  if (parent)
    parent->_rmVariable(this);
}

variable::Type variable::getType(void) const { return varType; }

void variable::setGlobal(bool isGlobal) { global = isGlobal; }

bool variable::isGlobal(void) const { return global; }

void variable::setPredef(bool predef) { this->predef = predef; }

bool variable::isPredef(void) const { return predef; }

void variable::setHidden(bool hidden) { this->hidden = hidden; }

bool variable::isHidden(void) const { return hidden; }

void variable::assign(const variable * sc)
{
  if (parent) {
    assert(parent == sc->getParent());
  }
}

void variable::init(void)
{
  forEachVar([](variable * var) { var->reset(); });
}

bool variable::operator==(const variable * other) const
{
  for (auto v : varMap) {
    auto it = other->varMap.find(v.first);
    if (it == other->varMap.end())
      return false;
    if (*v.second != it->second)
      return false;
  }
  return true;
}

bool variable::operator!=(const variable * other) const { return !(*this == other); }

variable* variable::operator=(const variable* other) {
  if(this == other)
    return this;
  assert(getType() == other->getType());
  assert(getVariables().size() == other->getVariables().size());

  for(auto var : other->getVariables()) {
    auto v = get(var->getLocalName());
    if(v) {
      (*v) = var;
    } else {
      //not the same type!
      assert(false);
    }
  }
  return this;
}

/*variable* variable::operator=(const argList& args) {
  assert(args.args.size() == getVariables().size());
  for(size_t i = 0; i < args.args.size(); i++) {
    *(varList[i]) = *args.args[i];
  }
  return this;
}*/

void variable::setParent(variable * parent)
{
  this->parent = parent;
}

variable * variable::getParent(void) const { return parent; }

std::string variable::getFullName(void) const { return parent ? parent->getFullName() + "." + getLocalName() : getLocalName(); }

std::string variable::getLocalName(void) const { return name; }

void variable::setName(std::string & name)
{
  assert(!name.empty());
  this->name = name;
}

std::string variable::getVisibleName(void) const
{
  if (parent) {
    if (parent->varType == V_PROG)
      // We don't want to include the global scope in the visible name
      return getLocalName();
    else
      return parent->getVisibleName() + "." + getLocalName();
  }
  return getLocalName();
}

unsigned int variable::getVariableId(void) const { return vid; }

void variable::_addVariable(variable * var)
{
  var->setParent(this);
  auto varName = var->getLocalName();
  varMap[varName] = var;
  varList.push_back(var);
}

void variable::_rmVariable(const variable * var)
{
  varMap.erase(var->getLocalName());
  auto it = std::find(varList.cbegin(), varList.cend(), var);
  varList.erase(it);
}

bool variable::hasVariables(void) const { return !getVariables().empty(); }

std::list<variable *> variable::getVariables(void) const { return std::list(varList.begin(), varList.end()); }

std::list<variable *> variable::getAllVariables(void) const
{
  std::list<variable *> res;
  for (auto var : varList) {
    res.push_back(var);
    auto subVars = var->getAllVariables();
    res.insert(res.end(), subVars.begin(), subVars.end());
  }
  return res;
}

std::list<variable *> variable::getAllVisibleVariables(bool excludeLocal) const
{
  std::list<variable *> res;
  for (auto var : varList) {
    auto name = var->getLocalName();
    if (var->predef || var->hidden) {
      // Internal variables and hidden variables are not visible
      continue;
    }
    auto isProcess = var->getType() == V_PROC;
    auto isEnumDeclaration = var->getType() == V_CMTYPE;
    auto isChannel = var->getType() == V_CHAN;
    if ((isProcess && excludeLocal) || isEnumDeclaration || isChannel) {
      // Process variables are not visible if we are excluding local variables
      // Enum declarations are not visible
      continue;
    }
    auto isUtype = var->getType() == V_STRUCT;
    if (isUtype) {
      auto subVars = var->getAllVisibleVariables();
      for (auto subVar : subVars) {
        auto isSubUType = subVar->getType() == V_STRUCT;
        if (isSubUType) {
          // If the sub-variable is a utype, we need to get all its sub-variables
          auto subVisibleVariables = subVar->getAllVisibleVariables();
          res.insert(res.end(), subVisibleVariables.begin(), subVisibleVariables.end());
        }
        else {
          // Otherwise, we just add the sub-variable to the list of visible variables
          res.push_back(subVar);
        }
      }
      continue;
    }
    if (name != "") {
      // If the variable has a name, we add it to the list of visible variables
      res.push_back(var);
    }
    auto subVars = var->getAllVisibleVariables();
    res.insert(res.end(), subVars.begin(), subVars.end());
  }
  // Sort the list of variables based on their local name
  res.sort([](variable * a, variable * b) { return a->getLocalName() < b->getLocalName(); });
  return res;
}

std::vector<std::shared_ptr<statePredicate>> variable::getPredicates() const{
  std::vector<std::shared_ptr<statePredicate>> result;
  auto visibleVariables = getAllVisibleVariables();
  for (auto var : visibleVariables) {
    auto predicates = var->getPredicates();
    for (auto pred : predicates) {
      result.push_back(pred);
    }
  }
  return result;
}


variable::operator std::string(void) const
{
  std::string res = "";
  for (auto var : varList)
    res += std::string(*var);
  return res;
}

void variable::print(void) const
{
  forEachVar([](variable * var) { var->print(); });
}

void variable::printTexada(void) const
{
  forEachVar([](variable * var) { var->printTexada(); });
}

void variable::printCSV(std::ostream & out) const
{
  forEachVar([&out](variable * var) { var->printCSV(out); });
}

void variable::printCSVHeader(std::ostream & out) const
{
  forEachVar([&out](variable * var) { var->printCSVHeader(out); });
}

float variable::delta(const variable * v2, bool considerInternalVariables) const
{
  if (v2 == nullptr)
    return 1;
  auto vars = considerInternalVariables ? getVariablesList() : getAllVisibleVariables();
  float res = 0;
  if (vars.empty())
    return 0;
  for (auto var : vars) {
    auto localName = var->getLocalName();
    auto v = v2->get(localName);
    auto delta_val = var->delta(v, considerInternalVariables);
    res += delta_val;
  }
  return res / vars.size();
}

void variable::printDelta(const variable * v2, bool considerInternalVariables) const
{
  auto vars = considerInternalVariables ? getVariablesList() : getAllVisibleVariables();
  for (auto var : vars) {
    auto localName = var->getLocalName();
    auto v = v2->get(localName);
    var->printDelta(v, considerInternalVariables);
  }
}

std::list<variable *> variable::getDelta(const variable * v2, bool considerInternalVariables) const
{
  std::list<variable *> res;
  auto vars = considerInternalVariables ? getVariablesList() : getAllVisibleVariables();
  for (auto var : vars) {
    auto v = v2->get(var->getLocalName());
    auto deltaValues = var->getDelta(v, considerInternalVariables);
    if (!deltaValues.empty()) {
      res.insert(res.end(), deltaValues.begin(), deltaValues.end());
    }
  }
  return res;
}

variable * variable::getVariableImpl(const std::string & name) const
{
  size_t pos = name.find(".");
  if (pos != std::string::npos) {
    auto subScope = name.substr(0, pos);
    variable * var = getVariableImpl(subScope);
    if(var == nullptr) {
      std::cout << subScope << " not found. " << std::endl;
      assert(false);
    }
    auto next = std::string(name).erase(0, pos + std::string(".").length());
    return var->getVariableImpl(next);
  }

  std::unordered_map<std::string, variable *>::const_iterator resIt = varMap.find(name);
  if (resIt != varMap.cend())
    return resIt->second;

  variable * var = nullptr;
  if (parent)
    var = parent->getVariableImpl(name);
  else {
    bool found = false;
    for (auto scope : varList) {
      auto v = scope->getVariableDownScoping(name);
      if (v) {
        if (found)
          throw std::runtime_error("Variable " + name + " is ambiguous, and is defined in multiple scopes.");
        found = true;
        var = v;
      }
    }
  }

  return var;
}

variable * variable::getVariableDownScoping(const std::string & name) const
{
  std::unordered_map<std::string, variable *>::const_iterator resIt = varMap.find(name);
  if (resIt != varMap.cend())
    return resIt->second;

  variable * var = nullptr;
  for (auto scope : varList) {
    auto v = scope->getVariableDownScoping(name);
    if (v) {
      var = v;
      break;
    }
  }
  return var;
}


std::unordered_map<std::string, variable *> variable::getVariablesMap(void) const { return varMap; }

std::list<variable *> variable::getVariablesList(void) const { return std::list<variable*>(varList.begin(), varList.end()); }

std::vector<variable *> variable::getVariablesVector(void) const { return varList; }

void variable::clearVariables(void)
{
  varList.clear();
  varMap.clear();
}

void variable::reset(void)
{
  forEachVar([](variable * var) { var->reset(); });
}

bool variable::isSame(const variable * other, bool considerInternalVariables) const
{
  auto deltaVal = this->delta(other, considerInternalVariables);
  auto threshold = 0.0000000001;
  return deltaVal < threshold;
}

unsigned long variable::hash(void) const
{
  auto _size = this->size();
  byte * data = new byte[_size];
  hash(data);
  auto res = std::hash<std::string_view>{}(std::string_view(data, _size));
  delete data;
  return res;
}

void variable::hash(byte * payload) const
{
  unsigned long offset = 0;
  for (auto var : varList) {
    var->hash(payload + offset);
    offset += var->size();
  }
}

size_t variable::size(void) const
{
  size_t res = 0;
  for (auto var : varList)
    res += var->size();
  return res;
}



/*************************************************************************************************/