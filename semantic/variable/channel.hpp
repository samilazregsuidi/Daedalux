#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <list>

#include "primitiveVariable.hpp"

class channel : public primitiveVariable {
public:
	channel(variable* parent, const chanSymNode* sym, unsigned int bound =  0);

	channel(const channel* other);

	variable* deepCopy(void) const override;

	~channel() override;

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

	void print(void) const override;

	void printTexada(void) const override;

private:
	void len(byte newLen);
};

class channelField : public primitiveVariable {
public:
	channelField(variable* parent, const varSymNode* sym, unsigned int fieldNumber, unsigned int messageIndex = 0, unsigned int index =  0);

	variable* deepCopy(void) const override;
};


class CIDVar : public primitiveVariable {
public:
	CIDVar(variable* parent, const cidSymNode* sym, unsigned int bound =  0);

	variable* deepCopy(void) const override;

	channel* getRefChannel(void) const;
	
	void setRefChannel(channel* newRef);

	void assign(const variable* sc) override;

private:
	channel* ref;
};

#endif