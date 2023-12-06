#include "channel.hpp"

#include "payload.hpp"

#include "chanSymNode.hpp"
#include "cidSymNode.hpp"

#include "initState.hpp"

channel::channel(const chanSymNode* chanSym, unsigned int index)
	: primitiveVariable(chanSym, index)
{
	if(chanSym->getBound() > 1)
		name += "["+std::to_string(index)+"]";

	if(chanSym->getCapacity() > 0){
		rawBytes++;
		for(int i = 0; i < chanSym->getCapacity(); ++i){
			unsigned int fieldIndex = 0;
			for(auto typeSym: chanSym->getTypeList()){
				for(unsigned int j = 0; j < typeSym->getBound(); ++j){
					//auto msgField = new channelField(typeSym, fieldIndex++, i, j);
					auto addedVars = initState::addVariables(this, typeSym);
				}
			}
		}
	} else {
		unsigned int fieldIndex = 0;
		for(auto typeSym: chanSym->getTypeList()){
			for(unsigned int j = 0; j < typeSym->getBound(); ++j){
				//auto msgField = new channelField(typeSym, fieldIndex++, 0, j);
				initState::addVariables(this, typeSym);
			}
		}
	}
}

channel::channel(const channel* other) 
	: primitiveVariable(other)
{
	assert(getSizeOf() == other->getSizeOf());
}

variable* channel::deepCopy(void) const{
	return new channel(this);
}

channel::~channel() {
}

size_t channel::getSizeOf(void) const {
	return variable::getSizeOf();
}

void channel::reset(void) {
	for(auto field : getVariables())
		field->reset();
}

//int return type for executability check?
void channel::send(const std::list<const variable*>& args) {
	
	auto argIt = args.cbegin();

	for(auto field : varList) {
		
		//field->print();
		//(*argIt)->print();

		*dynamic_cast<primitiveVariable*>(field) = *dynamic_cast<const primitiveVariable*>(*argIt++);

		//field->print();
	
	}

	if(!isRendezVous())
		len(len()+1);
}

void channel::receive(const std::list<variable*>& rargs) {

	auto rargIt = rargs.begin();

	for(auto field : varList) {
		
		auto rarg = (*rargIt);
		//rarg->print();
		//field->print();

		if(rarg->getLocalName() != std::string("_")) {
			*dynamic_cast<primitiveVariable*>(*rargIt++) = *dynamic_cast<primitiveVariable*>(field);
		}
		//rarg->print();
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
	return getPayload()->getValue<byte>(getOffset());
}

void channel::len(byte newLen) {
	if(!isRendezVous()) {
		assert(newLen < getCapacity());
		getPayload()->setValue<byte>(getOffset(), newLen);
	}
}

byte channel::getCapacity(void) const {
	return dynamic_cast<const chanSymNode*>(varSym)->getCapacity();
}

bool channel::operator == (const variable* other) const {
	return variable::operator==(other);
}

bool channel::operator != (const variable* other) const {
	return variable::operator!=(other);
}

channel::operator std::string(void) const {

}

void channel::print(void) const {

}

void channel::printTexada(void) const {

}

void channel::printCSV(std::ostream &out) const {}

void channel::printCSVHeader(std::ostream &out) const {}


/**************************************************************************************************/

/*channelField::channelField(const varSymNode* sym, unsigned int fieldNumber, unsigned int messageIndex, unsigned int index)
	: primitiveVariable(sym, index)
{
	name = ".("+sym->getTypeName()+")m" + std::to_string(messageIndex) + ".f" + std::to_string(fieldNumber) + name;
}*/

/**************************************************************************************************/

CIDVar::CIDVar(const cidSymNode* sym, unsigned int bound) 
	: primitiveVariable(sym, bound)
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
	getPayload()->setValue<channel*>(getOffset(), newRef);
}

void CIDVar::assign(const variable* sc) {
	variable::assign(sc);
	if(ref) {
		ref = dynamic_cast<channel*>(sc->getVariable(ref->getLocalName()));
		assert(ref);
	}
}

CIDVar::operator std::string(void) const {
	assert(false);
}

void CIDVar::print(void) const {
	assert(false);
}