#ifndef SCALAR_VARIABLE_IMPL_H
#define SCALAR_VARIABLE_IMPL_H

#include "scalarVarInt.hpp"
#include "payload.hpp"

#include <limits>
#include <cstring>

/**
 * @brief A templated class to represent a scalar variable with a T value and a variable::Type type
*/

template <typename T, variable::Type E> class scalar : public scalarInt {
public:

  scalar(T initValue)
    : scalarInt("", E)
    , initValue(initValue)
    , value(initValue)
  {}

  scalar(const std::string& name, T initValue = 0)
    : scalarInt(name, E) 
    , initValue(initValue)
    , value(initValue)
  {}

  scalar(const scalar<T, E>& other)
    : scalarInt(other)
    , initValue(other.initValue)
    , value(other.value)
  {}

  scalar<T, E>* deepCopy(void) const override {
    return new scalar<T, E>(*this);
  }

  size_t size(void) const override {
    assert(varList.size() == 0);
    return sizeof(T);
  }

  size_t hash(void) const override {
    return value;
  }

  void hash(byte* payload) const override {
    memcpy(payload, &value, size());
  }

  /****************************************************/

  virtual void setIntValue(int value) override {
    if constexpr(std::is_const<T>::value) {
      assert(false);
    } else {
      setValue(value);
    }
  }

  template<typename U = T> typename std::enable_if<!std::is_const<U>::value, void>::type setValue(T newValue) {
    //assert(getPayload());
    assert(newValue >= std::numeric_limits<T>::min());
    assert(newValue <= std::numeric_limits<T>::max());
    value = newValue;
  }

  T getValue(void) const {
    return value;
  }

  int getIntValue(void) const override {
    return getValue();
  }

  void setInitValue(T newValue) {
    initValue = newValue;
  }

  void init(void) override {
    if constexpr(!std::is_const<T>::value)
      setValue(initValue);
  }

  void reset(void) override {
    if constexpr(!std::is_const<T>::value)
      setValue(initValue);
  }

  variable* operator=(const variable* other) override {
    auto var = dynamic_cast<const scalar<T, E>*>(other);
    if (var) {
      if constexpr(!std::is_const<T>::value)
        setValue(var->getValue());
    } else
      assert(false);
    return this;
  }

  template<typename U = T> typename std::enable_if<!std::is_const<U>::value && !std::is_same<U, bool>::value, T>::type operator=(T rvalue) {
    setValue(rvalue);
    return getValue();
  }

  
  template<typename U = T> typename std::enable_if<!std::is_const<U>::value && !std::is_same<U, bool>::value, T>::type operator++(void) {
    T temp = getValue();
    if(temp + 1 <= std::numeric_limits<T>::max())
      setValue(++temp);
    return temp;
  }

  template<typename U = T> typename std::enable_if<!std::is_const<U>::value && !std::is_same<U, bool>::value, T>::type operator--(void) {
    T temp = getValue();
    if(temp - 1 >= std::numeric_limits<T>::min())
      setValue(--temp);
    return temp;;
  }

  template<typename U = T> typename std::enable_if<!std::is_const<U>::value && !std::is_same<U, bool>::value, T>::type operator++(int) {
    T temp = getValue();
    if(temp + 1 <= std::numeric_limits<T>::max())
      setValue(temp + 1);
    return temp;
  }

  template<typename U = T> typename std::enable_if<!std::is_const<U>::value && !std::is_same<U, bool>::value, T>::type operator--(int) {
    T temp = getValue();
    if(temp - 1 >= std::numeric_limits<T>::min())
      setValue(temp - 1);
    return temp;
  }

  bool operator==(const variable * other) const override {
    auto var = dynamic_cast<const scalar<T, E>*>(other);
    if(var)
      return getValue() == var->getValue();
    return false;
  }

  bool operator==(int value) const override {
    return getValue() == static_cast<T>(value);
  }

  bool operator!=(const variable * other) const override {
    return !(*this == other);
  }

  bool operator!=(int value) const override {
    return getValue() != static_cast<T>(value);
  }

  virtual operator T(void) const {
    return getValue();
  }

  float delta(const variable * other, bool considerInternalVariables) const override {
    auto cast = dynamic_cast<const scalar<T, E> *>(other);
    if (!cast)
      return 1;

    T value = getValue();
    T otherValue = cast->getValue();

    float diff = std::abs(float(value - otherValue));
    auto delta = 1.0 - (1.0 / (diff + 1.0));
    return delta;
  }

  std::list<variable *> getDelta(const variable * other, bool considerInternalVariables) const override {
    auto cast = dynamic_cast<const scalar<T, E> *>(other);
    if (!cast)
      return std::list<variable *>();

    std::list<variable *> res;
    auto delta = this->delta(cast, considerInternalVariables);

    if (delta > 0.00000001) {
      res.push_back(this->deepCopy());
    }
    return res;
  }

  void printDelta(const variable * other, bool considerInternalVariables) const override {
    auto cast = dynamic_cast<const scalar<T, E> *>(other);
    if (!cast)
      return;

    if (!this->isSame(other, considerInternalVariables)) {
      auto name = getFullName();
      T value = this->getValue();
      T otherValue = cast->getValue();
      auto OtherName = cast->getFullName();
      auto delta = this->delta(cast, considerInternalVariables);
      printf("%s = %d, %s = %d, delta = %f\n", name.c_str(), value, OtherName.c_str(), otherValue, delta);
    }
  }

  operator std::string(void) const override {
    auto value = getValue();
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%-23s = %d\n", getFullName().c_str(), value);

    // res += variable::operator std::string();
    return buffer;
  }

  //operator int(void) const;

  void print(void) const override {
    auto name = std::string(*this);
    printf("%s", name.c_str());
  }

  void printTexada(void) const override {
    if (isPredef())
      return;

    auto value = getValue();
    printf("%s = %d\n", getFullName().c_str(), value);

    variable::printTexada();
  }

  void printCSV(std::ostream & out) const override {
    if (isPredef())
      return;

    out << getFullName() + ",";
    variable::printCSVHeader(out);
  }

  void printCSVHeader(std::ostream & out) const override {
    if (isPredef())
      return;

    auto value = getValue();
    out << std::to_string(value) + ",";

    variable::printCSV(out);
  }

public:
  T initValue;
  T value;
};

typedef scalar<unsigned char, variable::Type::V_BIT> bitVar;
typedef scalar<bool, variable::Type::V_BOOL> boolVar;
typedef scalar<unsigned char, variable::Type::V_BYTE> byteVar;
typedef scalar<unsigned char, variable::Type::V_PID> pidVar;
typedef scalar<short, variable::Type::V_SHORT> shortVar;
typedef scalar<unsigned short, variable::Type::V_USHORT> ushortVar;
typedef scalar<int, variable::Type::V_INT> intVar;
typedef scalar<unsigned int, variable::Type::V_UINT> uintVar;
typedef scalar<long, variable::Type::V_LONG> longVar;
typedef scalar<unsigned long, variable::Type::V_ULONG> ulongVar;
typedef scalar<float, variable::Type::V_FLOAT> floatVar;
typedef scalar<double, variable::Type::V_DOUBLE> doubleVar;

#endif