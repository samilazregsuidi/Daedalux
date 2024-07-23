#include "channelVar.hpp"

#include "payload.hpp"

#include "chanSymNode.hpp"
#include "cidSymNode.hpp"

#include "initState.hpp"

channel::channel(const std::string& name, bool rendezVous)
	: queueVar(name, variable::V_CHAN)
	, rendezVous(rendezVous)
{
}

channel::channel(const channel& other) 
	: queueVar(other)
	, rendezVous(other.rendezVous)
{
	assert(size() == other.size());
}

channel* channel::deepCopy(void) const{
	return new channel(*this);
}

//int return type for executability check?
void channel::send(const paramList& args) {
	push(args);
}

//TODO : Make it work for a dynamic channel, add stack management

bool channel::isReceivable(const paramList& rargs) const {
	if(!isRendezVous() && len() == getCapacity())
		return false;

	auto fields = front()->getVariablesVector();
	assert(fields.size() == rargs.size());
	
	for(size_t i = 0; i < rargs.size(); ++i){
		if(rargs[i]->type == param::Type::VAL){
			if(rargs[i]->getIntValue() != (dynamic_cast<scalarInt*>(fields[i]))->getIntValue())
				return false;
		}
	}

	return true;
}

void channel::receive(paramList& rargs) {

	auto fields = front()->getVariablesVector();
	assert(fields.size() == rargs.size());
	
	rargs.out(front());
}

bool channel::isRendezVous(void) const {
	return rendezVous;
}

byte channel::len(void) const {
	if(isRendezVous())
		return 0;
	return queueVar::len();
}

byte channel::getCapacity(void) const {
	if(isRendezVous())
		return 0;
	return queueVar::capacity();
}

bool channel::operator == (const variable* other) const {
	return variable::operator==(other);
}

bool channel::operator != (const variable* other) const {
	return variable::operator!=(other);
}

float channel::delta(const variable* other, bool considerOtherVariable) const {
	if(isRendezVous())
		return 0;
	assert(false);
	return variable::delta(other, considerOtherVariable);
}

channel::operator std::string(void) const {
	return "";
}

void channel::print(void) const {}

void channel::printTexada(void) const {}

void channel::printCSV(std::ostream &out) const {}

void channel::printCSVHeader(std::ostream &out) const {}


/**************************************************************************************************/

/*channelField::channelField(const varSymNode* sym, unsigned int fieldNumber, unsigned int messageIndex, unsigned int index)
	: primitiveVariable(sym, index)
{
	name = ".("+sym->getTypeName()+")m" + std::to_string(messageIndex) + ".f" + std::to_string(fieldNumber) + name;
}*/

/**************************************************************************************************/

CIDVar::CIDVar(const std::string& name, unsigned char initValue) 
	: scalar<unsigned char, variable::V_CID>(name, initValue)
	, ref(nullptr)
{}

CIDVar* CIDVar::deepCopy(void) const{
	CIDVar* copy = new CIDVar(*this);
	return copy;
}

std::string CIDVar::getRefChannel(void) const {
	return ref;
}
	
void CIDVar::setRefChannel(const std::string& newRef) {
	ref = newRef;
}

CIDVar::operator std::string(void) const {
	assert(false);
}

void CIDVar::print(void) const {
	assert(false);
}