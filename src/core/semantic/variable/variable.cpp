#include "variable.hpp"

#include <iterator>

#include "payload.hpp"
#include "process.hpp"

#include "boolVariable.hpp"
#include "channel.hpp"
#include "mtypeVariable.hpp"
#include "utypeVariable.hpp"

#include "argExpr.hpp"
#include "constExpr.hpp"
#include "varExpr.hpp"

unsigned int padding(const varSymNode * varSym)
{
  switch (varSym->getType()) {
  case symbol::T_BIT:
  case symbol::T_BOOL:
  case symbol::T_BYTE:
  case symbol::T_CHAN:
  case symbol::T_MTYPE:
  case symbol::T_PID:
    return 1;

  case symbol::T_SHORT:
    return 2;

  case symbol::T_CID:
  case symbol::T_INT:
    return 4;

  case symbol::T_UTYPE:
    return padding(*dynamic_cast<const utypeSymNode *>(varSym)->getUType()->getFields().cbegin());

  default:
    assert(false);
    return -1;
  }
  assert(false);
  return -1;
}

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
    return V_UTYPE;
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
    : name(name), parent(nullptr), vid(++vidCounter), varType(varType), rawBytes(0), offset(0), payLoad(nullptr),
      isHidden(false), isPredef(false)
{
}

variable::variable(const variable & other)
    : name(other.name), parent(other.parent), vid(other.vid), varType(other.varType), rawBytes(other.rawBytes),
      varMap(other.varMap), varList(other.varList), offset(0), payLoad(other.payLoad), isHidden(other.isHidden),
      isPredef(other.isPredef)
{
}

variable::variable(const variable * other) : variable(*other)
{
  auto nbVariables = other->getVariables().size();
  // auto otherSizeOf = other->getSizeOf();

  payLoad = nullptr;
  clearVariables();

  for (auto subVar : other->getVariables()) {
    _addVariable(subVar->deepCopy());
  }

  for (auto subVar : getVariables())
    subVar->assign(subVar);

  if (!parent) {
    setPayload(other->getPayload()->copy());
  }

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
  else {
    if (payLoad)
      delete payLoad;
  }
}

variable::Type variable::getType(void) const { return varType; }

bool variable::isGlobal(void) const
{
  assert(false);
  return false;
}

void variable::assign(const variable * sc)
{
  if (parent) {
    assert(parent == sc->getParent());
  }
  else {
    //?
    assert(false);
  }

  /*if(hasVariables()){
          std::list<variable*> newFields;
          for(auto varSubField : getVariables()) {
                  auto field = sc->getVariable(varSubField->getLocalName());
                  assert(field);
                  newFields.push_back(field);
          }
          varList = newFields;
  }*/
}

void variable::init(void)
{
  if (!payLoad) {
    if (parent) {
      parent->init();
      return;
    }
    setPayload(new payload(getSizeOf()));
  }
  for (auto var : varList)
    var->init();
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

void variable::setParent(variable * parent)
{
  this->parent = parent;
  if (parent) {
    offset = parent->getEndOffset();
    payLoad = parent->getPayload();
  }
  else {
    offset = 0;
    payLoad = nullptr;
  }
}

variable * variable::getParent(void) const { return parent; }

std::string variable::getFullName(void) const { return parent ? parent->getFullName() + "." + getLocalName() : getLocalName(); }

std::string variable::getLocalName(void) const { return name; }

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
  varList.erase(std::find(varList.begin(), varList.end(), var));
}

bool variable::hasVariables(void) const { return getVariables().size() > 0; }

std::list<variable *> variable::getVariables(void) const { return varList; }

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
    auto isProcess = dynamic_cast<process *>(var);
    auto isUtype = dynamic_cast<utypeVar *>(var);
    auto isEnumDeclaration = dynamic_cast<cmtypeVar *>(var);
    if (var->isPredef || var->isHidden) {
      // Internal variables and hidden variables are not visible
      continue;
    }
    if ((isProcess && excludeLocal) || isEnumDeclaration) {
      // Process variables are not visible if we are excluding local variables
      // Enum declarations are not visible
      continue;
    }
    if (isUtype) {
      auto subVars = var->getAllVisibleVariables();
      for (auto subVar : subVars) {
        auto isSubUType = dynamic_cast<utypeVar *>(subVar);
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

variable::operator std::string(void) const
{
  std::string res;
  for (auto var : varList)
    res += std::string(*var);
  return res;
}

void variable::print(void) const
{
  for (auto var : varList)
    var->print();
}

void variable::printTexada(void) const
{
  for (auto var : varList)
    var->printTexada();
}

void variable::printCSV(std::ostream & out) const
{
  for (auto var : varList)
    var->printCSV(out);
}

void variable::printCSVHeader(std::ostream & out) const
{
  for (auto var : varList)
    var->printCSVHeader(out);
}

void variable::printHexadecimal(void) const { payLoad->printHexadecimal(getOffset(), getSizeOf()); }

void variable::setPayload(payload * newPayLoad)
{
  assert(!payLoad);
  payLoad = newPayLoad;

  for (auto var : varList)
    var->setPayload(newPayLoad);
}

payload * variable::getPayload(void) const { return payLoad; }

unsigned long variable::hash(void) const { return payLoad ? payLoad->hash(getOffset(), getSizeOf()) : 0; }

float variable::delta(const variable * v2, bool considerInternalVariables) const
{
  if (v2 == nullptr)
    return 1;
  float res = 0;
  auto vars = considerInternalVariables ? varList : getAllVisibleVariables();
  for (auto var : vars) {
    auto name = var->getLocalName();
    auto v = v2->getVariable(name);
    auto delta = var->delta(v, considerInternalVariables);
    res += delta;
  }
  if (vars.size() == 0)
    return 0;
  return res / vars.size();
}

void variable::printDelta(const variable * v2, bool considerInternalVariables) const
{
  auto vars = considerInternalVariables ? varList : getAllVisibleVariables();
  for (auto var : vars) {
    auto name = var->getLocalName();
    auto v = v2->getVariable(name);
    var->printDelta(v, considerInternalVariables);
  }
}

std::list<variable *> variable::getDelta(const variable * v2, bool considerInternalVariables) const
{
  std::list<variable *> res;
  auto vars = considerInternalVariables ? varList : getAllVisibleVariables();
  for (auto var : vars) {
    auto v = v2->getVariable(var->getLocalName());
    auto delta = var->getDelta(v, considerInternalVariables);
    if (delta != std::list<variable *>()) {
      res.insert(res.end(), delta.begin(), delta.end());
    }
  }
  return res;
}

size_t variable::getOffset(void) const { return parent ? offset + parent->getOffset() : offset; }

size_t variable::getEndOffset(void) const { return offset + getSizeOf(); }

void variable::addRawBytes(size_t size) { rawBytes += size; }

variable * variable::getVariable(const std::string & name) const
{
  size_t pos = name.find(".");
  if (pos != std::string::npos) {
    auto subScope = name.substr(0, pos);
    variable * var = getVariable(subScope);
    if (var == nullptr) {
      throw std::runtime_error("Variable " + subScope + " not found.");
    }
    auto next = std::string(name).erase(0, pos + std::string(".").length());
    return var->getVariable(next);
  }

  std::map<std::string, variable *>::const_iterator resIt = varMap.find(name);
  if (resIt != varMap.cend())
    return resIt->second;

  variable * var = nullptr;
  if (parent)
    var = parent->getVariable(name);
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
  std::map<std::string, variable *>::const_iterator resIt = varMap.find(name);
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

channel * variable::getChannel(const std::string & name) const
{

  auto var = getVariable(name);

  if (!var)
    return nullptr;

  channel * chan = nullptr;
  if (var->getType() == variable::V_CID) {
    chan = dynamic_cast<CIDVar *>(var)->getRefChannel();
    assert(chan);
  }
  else {
    assert(var->getType() == variable::V_CHAN);
    chan = dynamic_cast<channel *>(var);
  }
  assert(chan);
  return chan;
}

std::map<std::string, variable *> variable::getVariablesMap(void) const { return varMap; }

size_t variable::getSizeOf(void) const
{
  size_t size = rawBytes;

  for (auto var : varList)
    size += var->getSizeOf();

  return size;
}

void variable::clearVariables(void)
{
  varList.clear();
  varMap.clear();
}

void variable::reset(void)
{
  for (auto subVar : varList)
    subVar->reset();
}

bool variable::isSame(const variable * other, bool considerInternalVariables) const
{
  auto delta = this->delta(other, considerInternalVariables);
  auto threshold = 0.0000000001;
  return delta < threshold;
}

/*************************************************************************************************/