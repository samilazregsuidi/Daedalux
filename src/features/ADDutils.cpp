#include "ADDutils.hpp"

#include <assert.h>

bool implies(const ADD& a, const ADD& b) {
	assert(a || b);

	if(!b) return true;
	if(!a) return isTautology(b);

	return (a & ~b).IsZero();
}

bool isTautology(const ADD& fct) {
	return fct && (!fct.IsZero());
}