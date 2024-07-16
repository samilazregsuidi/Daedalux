#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <list>

#include "queueVar.hpp"
#include "paramList.hpp"

class channel : public queueVar {
public:
	channel(const std::string& name, bool rendezVous = false);

	channel(const channel& other);

	channel* deepCopy(void) const override;

	bool operator == (const variable* other) const override;

	bool operator != (const variable* other) const override;

	void send(const paramList& args);

	bool isReceivable(const paramList& rargs) const;

	void receive(paramList& rargs);

	bool isRendezVous(void) const;

	byte len(void) const;

	byte getCapacity(void) const;

	float delta(const variable* other, bool considerOtherVariable = true) const override;

	operator std::string(void) const override;

	void print(void) const override;

	void printTexada(void) const override;

	void printCSV(std::ostream &out) const override;

	void printCSVHeader(std::ostream &out) const override;

private:
	bool rendezVous;
};

#include "scalarVar.hpp"

class CIDVar : public scalar<unsigned char, variable::V_CID> {
public:
	CIDVar(const std::string& name, unsigned char initValue = 0);

	CIDVar* deepCopy(void) const override;

	std::string getRefChannel(void) const;
	
	void setRefChannel(const std::string& newRef);

	operator std::string(void) const override;

	void print(void) const override;

private:
	std::string ref;
};

#endif