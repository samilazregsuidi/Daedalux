#ifndef PRIMITIVE_VARIABLE_H
#define PRIMITIVE_VARIABLE_H

#include "variable.hpp"
#include "payload.hpp"

#include <limits>

/**
 * @brief A templated class to represent a primitive variable
*/

template <class T> class primitive : public variable {
public:

  primitive(const std::string& name, Type varType, T initValue)
    : variable(varType, name) 
    , initValue(initValue)
    , value(0)
  {}

  primitive(const primitive<T> & other)
    : variable(other)
    , initValue(other.initValue)
    , value(other.value)
  {}

  primitive(const primitive<T> * other)
    : variable(other)
    , initValue(other->initValue)
    , value(other->value)
  {}

  primitive<T>* deepCopy(void) const override {
    return new primitive<T>(this);
  }

  size_t getSizeOf(void) const override {
    return sizeof(T);
  }

  /****************************************************/

  virtual void setValue(T newValue) {
    //assert(getPayload());
    assert(newValue >= std::numeric_limits<T>::min());
    assert(newValue <= std::numeric_limits<T>::max());
    value = newValue;
    
    if(getPayload()) {
      getPayload()->setValue<T>(getOffset(), newValue);
    
      assert(getValue() == newValue);
    }
  }

  virtual T getValue(void) const {
    //assert(getPayload());
    if(getPayload()) {
      auto res = getPayload()->getValue<T>(getOffset());
      assert(res == value);
    }

    return value;
  }

  virtual void setInitValue(T newValue) {
    initValue = newValue;
  }

  virtual void init(void) override {
    //assert(getPayload());
    setValue(initValue);
  }

  virtual void reset(void) override {
    setValue(initValue);
  }

  variable* operator=(const variable* other) override {
    auto var = dynamic_cast<const primitive<T> *>(other);
    if (var) {
      *this = var->getValue();
    } else
      assert(false);
    return this;
  }

  virtual T operator=(T& rvalue) {
    setValue(rvalue);
    return getValue();
  }

  virtual T operator=(const argList& rvalue) {
    
    try {
      auto rvalueArg = dynamic_cast<const arg<T>&>(rvalue);
      T argVal = rvalueArg.value;
      *this = argVal;

    } catch (std::bad_cast& e) {
      assert(false);
    }

    return *this;
  }

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

    float value = getPayload()->getValue<T>(getOffset());
    float otherValue = cast->getPayload()->getValue<T>(cast->getOffset());

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
      auto otherValue = cast->getPayload()->getValue<T>(cast->getOffset());
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
    if (isPredef())
      return;

    auto value = getPayload()->getValue<T>(getOffset());
    printf("%s = %d\n", getFullName().c_str(), value);

    variable::printTexada();
  }

  virtual void printCSV(std::ostream & out) const override {
    assert(getPayload());
    if (isPredef())
      return;

    out << getFullName() + ",";
    variable::printCSVHeader(out);
  }

  virtual void printCSVHeader(std::ostream & out) const override {
    assert(getPayload());
    if (isPredef())
      return;

    auto value = getPayload()->getValue<T>(getOffset());
    out << std::to_string(value) + ",";

    variable::printCSV(out);
  }

protected:
  T initValue;
  T value;
};

class bitVar : public primitive<unsigned char> {
public:
  bitVar(unsigned char initValue = 0)
  : primitive<unsigned char>("", variable::V_BIT, initValue)
  {}

  bitVar(const std::string& name, unsigned char initValue = 0) 
  : primitive<unsigned char>(name, variable::V_BIT, initValue)
  {}

  bitVar* deepCopy(void) const override {
    return new bitVar(*this);
  }
};

class byteVar : public primitive<unsigned char> {
public:
  byteVar(unsigned char initValue = 0)
  : primitive<unsigned char>("", variable::V_BYTE, initValue)
  {}

  byteVar(const std::string& name, unsigned char initValue = 0) 
  : primitive<unsigned char>(name, variable::V_BYTE, initValue)
  {}

  byteVar* deepCopy(void) const override {
    return new byteVar(*this);
  }
};

class shortVar : public primitive<short> {
public:
  shortVar(short initValue = 0)
  : primitive<short>("", variable::V_SHORT, initValue)
  {}

  shortVar(const std::string& name, short initValue = 0) 
  : primitive<short>(name, variable::V_SHORT, initValue)
  {}

  shortVar* deepCopy(void) const override {
    return new shortVar(*this);
  }
};

class ushortVar : public primitive<unsigned short> {
public:
  ushortVar(unsigned short initValue = 0)
  : primitive<unsigned short>("", variable::V_USHORT, initValue)
  {}

  ushortVar(const std::string& name, unsigned short initValue = 0) 
  : primitive<unsigned short>(name, variable::V_USHORT, initValue)
  {}

  ushortVar* deepCopy(void) const override {
    return new ushortVar(*this);
  }
};

class intVar : public primitive<int> {
public:
  intVar(int initValue = 0)
  : primitive<int>("", variable::V_INT, initValue)
  {}

  intVar(const std::string& name, int initValue = 0) 
  : primitive<int>(name, variable::V_INT, initValue)
  {}

  intVar* deepCopy(void) const override {
    return new intVar(*this);
  }
};

class uintVar : public primitive<unsigned int> {
public:
  uintVar(unsigned int initValue = 0)
  : primitive<unsigned int>("", variable::V_UINT, initValue)
  {}

  uintVar(const std::string& name, unsigned int initValue = 0) 
  : primitive<unsigned int>(name, variable::V_UINT, initValue)
  {}

  uintVar* deepCopy(void) const override {
    return new uintVar(*this);
  }
};

class longVar : public primitive<long> {
public:
  longVar( long initValue = 0)
  : primitive<long>("", variable::V_LONG, initValue)
  {}

  longVar(const std::string& name, long initValue = 0) 
  : primitive<long>(name, variable::V_LONG, initValue)
  {}

  longVar* deepCopy(void) const override {
    return new longVar(*this);
  }
};

class ulongVar : public primitive<unsigned long> {
public:
  ulongVar(unsigned long initValue = 0)
  : primitive<unsigned long>("", variable::V_ULONG, initValue)
  {}

  ulongVar(const std::string& name, unsigned long initValue = 0) 
  : primitive<unsigned long>(name, variable::V_ULONG, initValue)
  {}

  ulongVar* deepCopy(void) const override {
    return new ulongVar(*this);
  }
};

class floatVar : public primitive<float> {
public:
  floatVar(float initValue = 0)
  : primitive<float>("", variable::V_FLOAT, initValue)
  {}

  floatVar(const std::string& name, float initValue = 0) 
  : primitive<float>(name, variable::V_FLOAT, initValue)
  {}

  floatVar* deepCopy(void) const override {
    return new floatVar(*this);
  }
};

class doubleVar : public primitive<double> {
public:
  doubleVar(double initValue = 0)
  : primitive<double>("", variable::V_DOUBLE, initValue)
  {}

  doubleVar(const std::string& name, double initValue = 0) 
  : primitive<double>(name, variable::V_DOUBLE, initValue)
  {}

  doubleVar* deepCopy(void) const override {
    return new doubleVar(*this);
  }
};

#endif