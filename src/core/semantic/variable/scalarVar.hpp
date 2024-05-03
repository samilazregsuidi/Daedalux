#ifndef SCALAR_VARIABLE_IMPL_H
#define SCALAR_VARIABLE_IMPL_H

#include "scalarVarInt.hpp"
#include "payload.hpp"

#include <limits>

/**
 * @brief A templated class to represent a scalar variable with a T value and a variable::Type type
*/

template <class T, variable::Type type> class scalar : public scalarInt {
public:

  scalar(const std::string& name, T initValue)
    : scalarInt(name, type) 
    , initValue(initValue)
    , value(0)
  {}

  scalar(const scalar<T>& other)
    : scalarInt(other)
    , initValue(other.initValue)
    , value(other.value)
  {}

  scalar<T>* deepCopy(void) const override {
    return new scalar<T>(*this);
  }

  size_t getSizeOf(void) const override {
    assert(varList.size() == 0);
    return sizeof(T);
  }

  /****************************************************/

  void setValue(int newValue) override {
    //assert(getPayload());
    assert(newValue >= std::numeric_limits<T>::min());
    assert(newValue <= std::numeric_limits<T>::max());
    value = newValue;
    
    if(getPayload()) {
      getPayload()->setValue<T>(getOffset(), newValue);
    
      assert(getValue() == newValue);
    }
  }

  int getValue(void) const override {
    //assert(getPayload());
    if(getPayload()) {
      auto res = getPayload()->getValue<T>(getOffset());
      assert(res == value);
    }

    return value;
  }

  void setInitValue(T newValue) {
    initValue = newValue;
  }

  void init(void) override {
    //assert(getPayload());
    setValue(initValue);
  }

  void reset(void) override {
    setValue(initValue);
  }

  variable* operator=(const variable* other) override {
    auto var = dynamic_cast<const scalarInt *>(other);
    if (var) {
      setValue(var->getValue());
    } else
      assert(false);
    return this;
  }

  virtual template<typename U = T> typename std::enable_if<!std::is_const<U>::value && !std::is_same<U, bool>::value, T>::type operator=(T rvalue) {
    setValue(rvalue);
    return getValue();
  }

  
  virtual template<typename U = T> typename std::enable_if<!std::is_const<U>::value && !std::is_same<U, bool>::value, T>::type operator++(void) {
    T temp = getValue();
    if(temp + 1 <= std::numeric_limits<T>::max())
      setValue(++temp);
    return temp;
  }

  virtual template<typename U = T> typename std::enable_if<!std::is_const<U>::value && !std::is_same<U, bool>::value, T>::type operator--(void) {
    T temp = getValue();
    if(temp - 1 >= std::numeric_limits<T>::min())
      setValue(--temp);
    return temp;;
  }

  virtual template<typename U = T> typename std::enable_if<!std::is_const<U>::value && !std::is_same<U, bool>::value, T>::type operator++(int) {
    T temp = getValue();
    if(temp + 1 <= std::numeric_limits<T>::max())
      setValue(temp + 1);
    return temp;
  }

  virtual template<typename U = T> typename std::enable_if<!std::is_const<U>::value && !std::is_same<U, bool>::value, T>::type operator--(int) {
    T temp = getValue();
    if(temp - 1 >= std::numeric_limits<T>::min())
      setValue(temp - 1);
    return temp;
  }

  bool operator==(const variable * other) const override {
    auto var = dynamic_cast<const scalarInt *>(other);
    if(var)
      return getValue() == var->getValue();
    return false;
  }

  bool operator==(int value) const override {
    return getValue() == value;
  }

  bool operator!=(const variable * other) const override {
    return !(*this == other);
  }

  bool operator!=(int value) const override {
    return getValue() != value;
  }

  virtual operator T(void) const {
    return getValue();
  }

  float delta(const variable * other) const override {
    auto cast = dynamic_cast<const scalar<T> *>(other);
    if (!cast)
      return 1;

    float value = getValue();
    float otherValue = cast->getValue();

    float diff = std::abs(value - otherValue);
    auto delta = 1.0 - (1.0 / (diff + 1.0));
    return delta;
  }

  std::list<variable *> getDelta(const variable * other) const override {
    auto cast = dynamic_cast<const scalar<T> *>(other);
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
    auto cast = dynamic_cast<const scalar<T> *>(other);
    if (!cast)
      return;

    auto delta = this->delta(cast);

    if (delta > 0.00000001) {
      auto name = getFullName();
      auto value = getPayload()->getValue<T>(getOffset());
      auto otherValue = cast->getValue();
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

  void print(void) const override {
    auto name = std::string(*this);
    printf("%s", name.c_str());
  }

  void printTexada(void) const override {
    assert(getPayload());
    if (isPredef())
      return;

    auto value = getPayload()->getValue<T>(getOffset());
    printf("%s = %d\n", getFullName().c_str(), value);

    variable::printTexada();
  }

  void printCSV(std::ostream & out) const override {
    assert(getPayload());
    if (isPredef())
      return;

    out << getFullName() + ",";
    variable::printCSVHeader(out);
  }

  void printCSVHeader(std::ostream & out) const override {
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

typedef scalar<unsigned char, variable::V_BIT> bitVar;
typedef scalar<bool, variable::V_BOOL> boolVar;
typedef scalar<unsigned char, variable::V_BYTE> byteVar;
typedef scalar<short, variable::V_SHORT> shortVar;
typedef scalar<unsigned short, variable::V_USHORT> ushortVar;
typedef scalar<int, variable::V_INT> intVar;
typedef scalar<unsigned int, variable::V_UINT> uintVar;
typedef scalar<long, variable::V_LONG> longVar;
typedef scalar<unsigned long, variable::V_ULONG> ulongVar;
typedef scalar<float, variable::V_FLOAT> floatVar;
typedef scalar<double, variable::V_DOUBLE> doubleVar;

#endif