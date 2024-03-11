#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <list>

#include "stackVar.hpp"
#include "argVar.hpp"

class channel : private stackVar {
public:
	channel(const std::string& name, size_t capacity = 0);

	channel(const channel* other);

	channel* deepCopy(void) const override;

	bool operator == (const variable* other) const override;

	bool operator != (const variable* other) const override;

	float delta(const variable* v2) const override;

	void printDelta(const variable* v2) const override;

	void send(const argList& args);

	bool isReceivable(const std::list<arg>& rargs) const;

	void receive(const std::list<arg>& rargs);

	bool isRendezVous(void) const;

	byte len(void) const;

	byte getCapacity(void) const;

	operator std::string(void) const override;

	void print(void) const override;

	void printTexada(void) const override;

	void printCSV(std::ostream &out) const override;

	void printCSVHeader(std::ostream &out) const override;

private:
	void len(byte newLen);
};

class CIDVar : public primitive<unsigned char> {
public:
	CIDVar(const cidSymNode* sym, unsigned int bound =  0);

	CIDVar* deepCopy(void) const override;

	channel* getRefChannel(void) const;
	
	void setRefChannel(channel* newRef);

	void assign(const variable* sc) override;

	operator std::string(void) const override;

	void print(void) const override;

private:
	channel* ref;
};

#endif