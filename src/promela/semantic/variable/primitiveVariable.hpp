#ifndef PRIMITIVE_VARIABLE_H
#define PRIMITIVE_VARIABLE_H

#include "variable.hpp"

class variable;

struct arg {
  
  arg(variable * variable) 
    : type(VAR) { data.variable = variable; }
  
  arg(int value) 
    : type(VAL) { data.value = value; }
  
  enum {
    VAR,
    VAL
  } type;

  union {
    variable * variable;
    int value;
  } data;
};

/**
 * @brief A templated class to represent a primitive variable
*/

template <class T> class primitive : public variable {
public:

  primitive(const std::string& name, Type varType, T initValue = 0)
    : variable(varType) 
    , name(name)
    , initValue(initValue)
    , value(0)
    , global(false)
    , predef(false) 
  {}

  primitive(const primitive<T> & other)
    : variable(other)
    , initValue(other.initValue)
    , value(other.value)
    , global(other.global)
    , predef(other.predef)
  {}

  primitive(const primitive<T> * other)
    : variable(other)
    , initValue(other->initValue)
    , value(other->value) 
    , global(other->global)
    , predef(other->predef)
  {}

  variable * deepCopy(void) const override {
    return new primitive<T>(this);
  }

  bool isGlobal(void) const override {
    return global;
  }

  bool isPredef(void) const {
    return predef;
  }

  size_t getSizeOf(void) const override {
    return sizeof(T);
  }

  /****************************************************/

  virtual void setValue(T newValue) {
    assert(getPayload());
    assert(newValue >= std::numeric_limits<T>::min());
    assert(newValue <= std::numeric_limits<T>::max());
    value = newValue;
    getPayload()->setValue<T>(getOffset(), newValue);
    assert(getValue() == newValue);
  }

  virtual T getValue(void) const {
    assert(getPayload());
    auto res = getPayload()->getValue<T>(getOffset());
    assert(res == value);
    return res;
  }

  virtual void setInitValue(T newValue) {
    initValue = newValue;
  }

  virtual void init(void) override {
    assert(getPayload());
    setValue(initValue);
  }

  virtual void reset(void) override {
    setValue(initValue);
  }

  virtual T operator=(const primitive<T> & rvalue) {
    T res = rvalue.getValue();
    setValue(res);
    return res;
  }

  virtual T operator=(const arg & rvalue);

  virtual T operator++(void) {
    T temp = getValue();
    if(temp + 1 <= std::numeric_limits<T>::max())
      setValue(++temp);
    return temp;
  }

  virtual T operator--(void) {
    T temp = getValue();
    if(temp - 1 >= std::numeric_limits<T>::min())
      setValue(--temp);
    return temp;;
  }

  virtual T operator++(int) {
    T temp = getValue();
    if(temp + 1 <= std::numeric_limits<T>::max())
      setValue(temp + 1);
    return temp;
  }

  virtual T operator--(int) {
    T temp = getValue();
    if(temp - 1 >= std::numeric_limits<T>::min())
      setValue(temp - 1);
    return temp;
  }

  bool operator==(const variable * other) const override {
    auto var = dynamic_cast<const primitive<T> *>(other);
    if(var)
      return getValue() == var->getValue();
    return false;
  }

  bool operator==(T value) const {
    return getValue() == value;
  }

  bool operator!=(const variable * other) const override {
    return !(*this == other);
  }

  bool operator!=(T value) const {
    return getValue() != value;
  }

  operator T(void) const {
    return getValue();
  }

  float delta(const variable * other) const override {
    auto cast = dynamic_cast<const primitive<T> *>(other);
    if (!cast)
      return 1;

    float value = getPayload()->getValue(getOffset(), getType());
    float otherValue = cast->getPayload()->getValue(cast->getOffset(), cast->getType());

    float diff = std::abs(value - otherValue);
    auto delta = 1.0 - (1.0 / (diff + 1.0));
    return delta;
  }

  std::list<variable *> getDelta(const variable * other) const override {
    auto cast = dynamic_cast<const primitive<T> *>(other);
    if (!cast)
      return std::list<variable *>();

    std::list<variable *> res;
    auto delta = this->delta(cast);

    if (delta > 0.00000001) {
      res.push_back(this->deepCopy());
    }
    return res;
  }

  void printDelta(const variable * other) const override {
    auto cast = dynamic_cast<const primitive<T> *>(other);
    if (!cast)
      return;

    auto delta = this->delta(cast);

    if (delta > 0.00000001) {
      auto name = getFullName();
      auto value = getPayload()->getValue<T>(getOffset());
      auto otherValue = cast->getPayload()->getValue(cast->getOffset(), cast->getType());
      auto OtherName = cast->getFullName();
      printf("%s = %d, %s = %d, delta = %f\n", name.c_str(), value, OtherName.c_str(), otherValue, delta);
    }
  }

  operator std::string(void) const override {
    assert(getPayload());
    auto value = getPayload()->getValue<T>(getOffset());
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "0x%-4ld:   %-23s = %d\n", getOffset(), getFullName().c_str(), value);

    // res += variable::operator std::string();
    return buffer;
  }

  //operator int(void) const;

  virtual void print(void) const override {
    auto name = std::string(*this);
    printf("%s", name.c_str());
  }

  virtual void printTexada(void) const override {
    assert(getPayload());
    if (predef)
      return;

    auto value = getPayload()->getValue(getOffset(), getType());
    printf("%s = %d\n", getFullName().c_str(), value);

    variable::printTexada();
  }

  virtual void printCSV(std::ostream & out) const override {
    assert(getPayload());
    if (predef)
      return;

    out << getFullName() + ",";
    variable::printCSVHeader(out);
  }

  virtual void printCSVHeader(std::ostream & out) const override {
    assert(getPayload());
    if (predef)
      return;

    auto value = getPayload()->getValue(getOffset(), getType());
    out << std::to_string(value) + ",";

    variable::printCSV(out);
  }

protected:
  T initValue;
  T value;
  bool global;
  bool predef;
};

class bitVar : public primitive<unsigned char> {
public:
  bitVar(const std::string& name, unsigned char initValue = 0) 
  : primitive<unsigned char>(name, variable::V_BIT, initValue)
  {}
};

class byteVar : public primitive<unsigned char> {
public:
  byteVar(const std::string& name, unsigned char initValue = 0) 
  : primitive<unsigned char>(name, variable::V_BYTE, initValue)
  {}
};

class shortVar : public primitive<short> {
public:
  shortVar(const std::string& name, short initValue = 0) 
  : primitive<short>(name, variable::V_SHORT, initValue)
  {}
};

class ushortVar : public primitive<unsigned short> {
public:
  ushortVar(const std::string& name, unsigned short initValue = 0) 
  : primitive<unsigned short>(name, variable::V_USHORT, initValue)
  {}
};

class intVar : public primitive<int> {
public:
  intVar(const std::string& name, int initValue = 0) 
  : primitive<int>(name, variable::V_INT, initValue)
  {}
};

class uintVar : public primitive<unsigned int> {
public:
  uintVar(const std::string& name, unsigned int initValue = 0) 
  : primitive<unsigned int>(name, variable::V_UINT, initValue)
  {}
};

class longVar : public primitive<long> {
public:
  longVar(const std::string& name, long initValue = 0) 
  : primitive<long>(name, variable::V_LONG, initValue)
  {}
};

class ulongVar : public primitive<unsigned long> {
public:
  ulongVar(const std::string& name, unsigned long initValue = 0) 
  : primitive<unsigned long>(name, variable::V_ULONG, initValue)
  {}
};

class floatVar : public primitive<float> {
public:
  floatVar(const std::string& name, float initValue = 0) 
  : primitive<float>(name, variable::V_FLOAT, initValue)
  {}
};

class doubleVar : public primitive<double> {
public:
  doubleVar(const std::string& name, double initValue = 0) 
  : primitive<double>(name, variable::V_DOUBLE, initValue)
  {}
};

class process;

class PIDVar : public primitive<unsigned char> {
public:
  PIDVar(const std::string& name, process* ref);

  variable * deepCopy(void) const override;

  process * getRefProcess(void) const;

  void setRefProcess(process * newRef);

  void assign(const variable * sc) override;

private:
  process * ref;
};

#endif