#include <iostream>
#include <type_traits>

struct base {
	virtual void f(void) {
	};
};

template <typename T> struct var : base{
	var(T val = 0)
		: val(val)
	{}
	
	template<typename U = T> typename std::enable_if<!std::is_const<U>::value && !std::is_same<U, bool>::value, T>::type operator++(void) {
		return ++val;
	}
	
	T val;
};

typedef var<const bool> cbool;

int main() {

	var<bool> b(false);
	var<int> i(1);
	++i;
	cbool cb(true);
	var<const int> ci = 2;
	//++ci;
	
	std::cout<<cb.val<<std::endl;
}
