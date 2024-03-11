#ifndef STACK_VARIABLE_H
#define STACK_VARIABLE_H

#include "variable.hpp"

class stackVar : public variable {
public:
	stackVar(const std::string& name);

	stackVar(const stackVar & other);

	stackVar(const stackVar * other);

	variable* deepCopy(void) const override;

	void push_front(const argList& var);

	void push_back(const argList& var);

	variable* front(void) const;

	void pop_front(void);

	variable* back(void) const;

	void pop_back(void);

	bool empty(void) const;

	bool full(void) const;

	size_t len(void) const;

	size_t capacity(void) const;

	void clear(void);

	bool operator == (const variable* other) const override;

	bool operator != (const variable* other) const override;

	variable* operator=(const variable* other) override;

private:
	size_t front_i;
	size_t back_i;
	size_t length;
};

#endif