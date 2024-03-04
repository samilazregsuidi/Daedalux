#include "variable.hpp"

#include <iterator>

#include "payload.hpp"

#include "channelVar.hpp"

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

size_t variable::getLowerBound(variable::Type type) { 
  switch (type) {
  case V_BIT:
    return bounds<V_BIT>::min;
  case V_BOOL:
    return bounds<V_BOOL>::min;
  case V_BYTE:
    return bounds<V_BYTE>::min;
  case V_SHORT:
    return bounds<V_SHORT>::min;
  case V_INT:
    return bounds<V_INT>::min;
  case V_PID:
    return bounds<V_PID>::min;
  case V_MTYPE:
    return bounds<V_MTYPE>::min;
  case V_CID: 
    return bounds<V_CID>::min;
  default:
    assert(false);
    return -1;
  }
}

size_t variable::getUpperBound(variable::Type type) { 
  switch (type) {
  case V_BIT:
    return bounds<V_BIT>::max;
  case V_BOOL:
    return bounds<V_BOOL>::max;
  case V_BYTE:
    return bounds<V_BYTE>::max;
  case V_SHORT:
    return bounds<V_SHORT>::max;
  case V_INT:
    return bounds<V_INT>::max;
  case V_PID:
    return bounds<V_PID>::max;
  case V_MTYPE:
    return bounds<V_MTYPE>::max;
  case V_CID: 
    return bounds<V_CID>::max;
  default:
    assert(false);
    return -1;
  }
}

unsigned int variable::vidCounter = 0;

variable::variable(Type varType, const std::string & name)
    : name(name), parent(nullptr), vid(++vidCounter), varType(varType), rawBytes(0), offset(0), payLoad(nullptr),
      hidden(false), predef(false), global(false)
{
}

variable::variable(const variable & other)
    : name(other.name), parent(other.parent), vid(other.vid), varType(other.varType), rawBytes(other.rawBytes),
      varMap(other.varMap), varList(other.varList), offset(0), payLoad(other.payLoad), hidden(other.hidden),
      predef(other.predef), global(other.global)
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

void variable::setGlobal(bool isGlobal) { global = isGlobal; }

bool variable::isGlobal(void) const { return global; }

void variable::setPredef(bool isPredef) { predef = isPredef; }

bool variable::isPredef(void) const { return predef; }

void variable::setHidden(bool isHidden) { hidden = isHidden; }

bool variable::isHidden(void) const { return hidden; }

void variable::assign(const variable * sc)
{
  if (parent) {
    assert(parent == sc->getParent());
  }
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

float variable::delta(const variable * v2) const
{
  float res = 0;
  if (v2 == nullptr)
    return 1;

  for (auto var : varList) {
    auto name = var->getLocalName();
    //std::cout << "name: " << name << std::endl;
    auto v = v2->getVariable(name);
    res += var->delta(v);
  }

  return res / varList.size();
}

void variable::printDelta(const variable * v2) const
{
  for (auto var : varList) {
    auto v = v2->getVariable(var->getLocalName());
    var->printDelta(v);
  }
}

std::list<variable *> variable::getDelta(const variable * v2) const
{
  std::list<variable *> res;
  for (auto var : varList) {
    auto v = v2->getVariable(var->getLocalName());
    auto delta = var->getDelta(v);
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
  if(pos != std::string::npos) {
    auto subScope = name.substr(0, pos);
    variable * var = getVariable(subScope);
    if(var == nullptr) {
      std::cout << subScope << " not found. " << std::endl;
      assert(false);
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
        assert(!found);
        found = true;
        var = v;
      }
    }
  }

  return var;
}

variable* variable::getVariableDownScoping(const std::string& name) const {
	std::map<std::string, variable*>::const_iterator resIt = varMap.find(name);
	if(resIt != varMap.cend())
		return resIt->second;
	
	variable* var = nullptr;
	for (auto scope : varList) {
		auto v = scope->getVariableDownScoping(name);
		if(v) {
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

std::list<variable *> variable::getVariablesList(void) const { return varList; }

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

bool variable::isSame(const variable * other) const
{
  auto delta = this->delta(other);
  return delta < 0.0000000001;
}

/*************************************************************************************************/