#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <list>

#include "stackVar.hpp"
#include "argList.hpp"

class channel : public stackVar {
public:
	channel(const std::string& name, size_t capacity = 0);

	channel(const channel& other);

	channel* deepCopy(void) const override;

	bool operator == (const variable* other) const override;

	bool operator != (const variable* other) const override;

	float delta(const variable* v2) const override;

	void printDelta(const variable* v2) const override;

	void send(const argList& args);

	bool isReceivable(const argList& rargs) const;

	void receive(const argList& rargs);

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

#include "scalarVar.hpp"

class CIDVar : public scalar<unsigned char> {
public:
	CIDVar(const cidSymNode* sym, unsigned char initValue = 0);

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