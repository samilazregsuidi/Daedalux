#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <list>

#include "primitiveVariable.hpp"

class channel : public primitiveVariable {
public:
	channel(const chanSymNode* sym, unsigned int bound =  0);

	channel(const channel* other);

	variable* deepCopy(void) const override;

	~channel() override;

	size_t getSizeOf(void) const override;

	int operator = (const primitiveVariable& rvalue) override;

	int operator ++ (void) override;

	int operator -- (void) override;

	int operator ++ (int) override;

	int operator -- (int) override;

	bool operator == (const primitiveVariable& other) const override;

	bool operator != (const primitiveVariable& other) const override;

	void send(const std::list<const variable*>& args);

	void reset(void);

	void receive(const std::list<variable*>& rargs);

	bool isRendezVous(void) const;

	bool isEmpty(void) const;

	bool isFull(void) const;

	primitiveVariable* getField(unsigned int index) const;
	
	byte len(void) const;

	byte getCapacity(void) const;

	operator std::string(void) const override;

	void print(void) const override;

	void printTexada(void) const override;

	void printDaikon(std::ostream &out) const override;

private:
	void len(byte newLen);
};

class CIDVar : public primitiveVariable {
public:
	CIDVar(const cidSymNode* sym, unsigned int bound =  0);

	variable* deepCopy(void) const override;

	channel* getRefChannel(void) const;
	
	void setRefChannel(channel* newRef);

	void assign(const variable* sc) override;

	operator std::string(void) const override;

	void print(void) const override;

private:
	channel* ref;
};

#endif