#ifndef STACK_VARIABLE_H
#define STACK_VARIABLE_H

#include "variable.hpp"
#include "paramList.hpp"

class queueVar : public variable {
public:
	queueVar(const std::string& name, variable::Type type = variable::Type::V_QUEUE);

	queueVar(const queueVar & other);

	variable* deepCopy(void) const override;

	void push(const paramList& params);

	variable* front(void) const;

	void pop(void);

	variable* back(void) const;

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