#include "channel.hpp"

#include "payload.hpp"

#include "chanSymNode.hpp"
#include "cidSymNode.hpp"

channel::channel(variable* parent, const chanSymNode* chanSym, unsigned int index)
	: primitiveVariable(chanSym, parent, index)
{
	if(chanSym->getBound() > 1)
		name += "["+std::to_string(index)+"]";

	if(chanSym->getCapacity() > 0)
		sizeOf += 1;

	for(int i = 0; i < chanSym->getCapacity(); ++i){
		unsigned int fieldIndex = 0;
		for(auto typeSym: chanSym->getTypeList()){
			for(unsigned int j = 0; j < typeSym->getBound(); ++j){
				auto msgField = new channelField(this, typeSym, fieldIndex++, i, j);
				_addVariable(msgField);
			}
		}
	}
}

channel::channel(const channel* other) 
	: primitiveVariable(other)
{}

variable* channel::deepCopy(void) const{
	return new channel(this);
}

channel::~channel() {
}

void channel::reset(void) {
	getPayload()->reset();
}

//int return type for executability check?
void channel::send(const std::list<const variable*>& args) {
	
	auto argIt = args.cbegin();

	for(auto field : varList) {
		
		//field->print();
		//(*argIt)->print();

		*field = **argIt++;

		//field->print();
	
	}

	if(!isRendezVous())
		len(len()+1);
}

void channel::receive(const std::list<variable*>& rargs) {

	auto rargIt = rargs.begin();

	for(auto field : varList) {
		
		//(*rargIt)->print();
		//field->print();

		**rargIt++ = *field;
	}

	if(isRendezVous())
		reset();
	else 
		len(len()-1);
	
}

//to moove to variable class?
primitiveVariable* channel::getField(unsigned int index) const {
	assert(0 < index && index < varList.size());
	auto it = varList.begin();
	std::advance(it, index);
	return dynamic_cast<primitiveVariable*>(*it);
}

bool channel::isRendezVous(void) const {
	return getCapacity() == 0;
}

bool channel::isFull(void) const {
	return len() == getCapacity();
}

bool channel::isEmpty(void) const {
	return len() == 0;
}

byte channel::len(void) const {
	if(isRendezVous())
		return 0;
	return getPayload()->getValue<byte>(offset);
}

void channel::len(byte newLen) {
	if(!isRendezVous()) {
		assert(newLen < getCapacity());
		getPayload()->setValue<byte>(offset, newLen);
	}
}

byte channel::getCapacity(void) const {
	return dynamic_cast<const chanSymNode*>(varSym)->getCapacity();
}

int channel::operator = (const primitiveVariable& rvalue) {
	rvalue;
	assert(false);
}

int channel::operator ++ (void) {
	assert(false);
}

int channel::operator -- (void) {
	assert(false);
}

int channel::operator ++ (int) {
	assert(false);
}

int channel::operator -- (int) {
	assert(false);
}

bool channel::operator == (const primitiveVariable& other) const {
	other;
	assert(false);
}

bool channel::operator != (const primitiveVariable& other) const {
	other;
	assert(false);
}

void channel::print(void) const {

}

void channel::printTexada(void) const {

}

/**************************************************************************************************/

channelField::channelField(variable* parent, const varSymNode* sym, unsigned int fieldNumber, unsigned int messageIndex, unsigned int index)
	: primitiveVariable(sym, parent, index)
{
	name = ".("+sym->getTypeName()+")m" + std::to_string(messageIndex) + ".f" + std::to_string(fieldNumber) + name;
}

variable* channelField::deepCopy(void) const{
	variable* copy = new channelField(*this);
	return copy;
}

/**************************************************************************************************/

CIDVar::CIDVar(variable* parent, const cidSymNode* sym, unsigned int bound) 
	: primitiveVariable(sym, parent, bound)
	, ref(nullptr)
{}

variable* CIDVar::deepCopy(void) const{
	variable* copy = new CIDVar(*this);
	return copy;
}

channel* CIDVar::getRefChannel(void) const {
	return ref;
}
	
void CIDVar::setRefChannel(channel* newRef) {
	ref = newRef;
	getPayload()->setValue<channel*>(offset, newRef);
}

void CIDVar::assign(const variable* sc) {
	variable::assign(sc);
	if(ref) {
		ref = dynamic_cast<channel*>(sc->getVariable(ref->getLocalName()));
		assert(ref);
	}
}

