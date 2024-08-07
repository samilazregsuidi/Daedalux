#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <assert.h>
#include <algorithm>
#include <vector>

#include "symTable.hpp"
#include "symbol.hpp"
#include "symTabVisitor.hpp"
#include "intSymNode.hpp"
#include "pidSymNode.hpp"
#include "boolSymNode.hpp"
#include "sysSymNode.hpp"

#include "tdefSymNode.hpp"

symTable::symTable(const std::string& name, symTable* prev)
	: name(name)
	, prev(prev)
{
}

symTable* symTable::createSubTable(const std::string& name) {
	symTable* neww = new symTable(name, this);
	nexts.push_back(neww);
	//neww->parentNameSpace = this->parentNameSpace + this->name;
	return neww;
}

symTable::~symTable() {
	for(const auto& s : syms)
		delete s.second;

	for(auto& n : nexts)
		delete n;
}

std::string symTable::getNameSpace(void) const {
	return name;
}

std::string symTable::getFullNameSpace(void) const {
	return prev? prev->getFullNameSpace() + "." + name : name;
}

void symTable::setNameSpace(const std::string& name) {
	this->name = name;
}

void symTable::setPrevSymTab(symTable* symTab) {
	assert(prev != nullptr);
	prev = symTab;
}

symTable* symTable::prevSymTab(void) const {
	return prev;
}

void symTable::addNextSymTab(symTable* symTab) {
	symTab->prev = this;
	nexts.push_back(symTab);
}

symTable* symTable::getSubSymTab(const std::string& name) const {
	symTable* res = nullptr;
	for(auto tab : nexts) {
		if(tab->name == name)  
			return tab;
		res = tab->getSubSymTab(name);
		if(res) return res;
	}
	return nullptr;
}

bool symTable::isGlobal(void) const {
	return name == "global" && !prev;
}

bool symTable::isMultiSystems(void) const {
	return name == "system" && syms.size() > 1;
}

void symTable::print(int tab) const {
	
	std::cout<<name<<" table\n";
	for(const auto& s : syms) {
		for(int i = 0; i < tab; ++i)
			std::cout << "\t";
		std::cout << "[\t" << s.second->getName() << "\t;\t" << s.second->getTypeName() << "\t]\n";
	}

	for(const auto& n : nexts) {
		n->print(tab+1);
	}
}

symbol* symTable::lookup(const std::string& name) const {
	if(std::find(name.begin(), name.end(), '.') != name.end()) {
		auto pos = name.find('.');
		auto sub = name.substr(0, pos);
		auto res = syms.find(sub);
		if(res != syms.cend()) {
			auto sym = dynamic_cast<complexSymNode*>(res->second);
			assert(sym);
			auto symTable = sym->getSubSymTable();
			return symTable->lookup(name.substr(pos+1));
		}
		else {
			auto res = std::find_if(nexts.begin(), nexts.end(), [sub](symTable* tab) { return tab->getNameSpace() == sub; });
			assert(res != nexts.end());
			return (*res)->lookup(name.substr(pos+1));
		}
	}
	const auto& res = syms.find(name);
	return res == syms.cend()? nullptr : res->second;
}

/*std!::list<symbol*> symTable::getSymbols(symbol::Type type, unsigned int mask) const {
	
	auto res = prev? prev->getSymbols(type, mask): std::list<symbol*>();

	for(auto& s : syms)
		if(s.second->getType() == type && s.second->getMask() == mask)
			res.push_back(s.second);

	return res;
}*/

std::set<symbol*> symTable::getSymbols(void) const {
	std::set<symbol*> res;
	for(auto& s : syms)
		res.insert(s.second);

	return res;
}

std::set<symbol*> symTable::getSymbols(const symbol* left) const {
	
	auto res = prev? prev->getSymbols(left): std::set<symbol*>();

	for(auto& s : syms)
		if(left->castTo(s.second) && left->getMask() == s.second->getMask())
			res.insert(s.second);

	return res;
}

symbol* symTable::lookupGlobal(const std::string& name) const {
	auto sym = lookup(name);
	return sym? sym : (prev? prev->lookupGlobal(name): nullptr);
}

void symTable::insert(symbol* sym) {
	assert(syms.find(sym->getName()) == syms.end());
	syms[sym->getName()] = sym;
	sym->setSymTable(this);
}

void symTable::remove(const std::string& name) {
	assert(syms.find(name) != syms.end());
	syms.find(name)->second->detach();
	syms.erase(name);
}

void symTable::remove(const symbol* sym) {
	remove(sym->getName());
}

void symTable::clear(void) {
	for(auto sym : syms)
		sym.second->setSymTable(nullptr);

	syms.clear();
	nexts.clear();
	prev = nullptr;
	block = nullptr;
}

void symTable::setBlock(stmnt* block) {
	assert(block);
	this->block = block;
}
	
stmnt* symTable::getBlock(void) const {
	assert(block);
	return block;
}
	
void symTable::addPredefinedSym(symTable* tab) {
	symbol* predef = new intSymNode(0, "_");
	predef->setMask(symbol::WRITE_ACCESS | symbol::PREDEFINED); tab->insert(predef);
	
	predef = new intSymNode(0, "_nr_pr");
	predef->setMask(symbol::READ_ACCESS | symbol::PREDEFINED); tab->insert(predef);

	/*predef = new pidSymNode(0, "_pid");
	predef->setMask(symbol::READ_ACCESS | symbol::PREDEFINED); tab->insert(predef);*/

	predef = new pidSymNode(0, "_last");
	predef->setMask(symbol::READ_ACCESS | symbol::PREDEFINED); tab->insert(predef);

	predef = new boolSymNode(0, "np_");
	predef->setMask(symbol::READ_ACCESS | symbol::PREDEFINED); tab->insert(predef);

	predef = new intSymNode(0, "k");
	predef->addToMask(symbol::PREDEFINED); tab->insert(predef);
}

void symTable::acceptVisitor(symTabVisitor* visitor){
	visitor->visitTab(this);
}

void symTable::acceptVisitor(symTabConstVisitor* visitor) const {
	visitor->visitTab(this);
}

void symTable::printGraphViz(std::ofstream& file) const {
	/*if(syms.empty())
		return;*/

	if(!prev){
		file << "digraph symbol_table {\n";
		file << "\t rankdir = LR\n";
	}

	file << "\t" << getNameSpace() << "[ shape = record, label = \"<f0> "<< getNameSpace();
	for(auto s : getSymbols()) {
		file << " | ";
		s->printGraphViz(file);
	}

	file << "\"]\n\n";

	for(auto n : nexts) {
		if(n->getSymbols().empty())
			continue;
		n->printGraphViz(file);
		file << "\t" << getNameSpace() << ":f0 -> " << n->getNameSpace() << ":f0\n\n";
	}

	for(auto s : getSymbols<tdefSymNode*>()) {
		file << "\t" << s->getName() << "[ shape = record, label = \"";
		unsigned int i = 0;
		for(auto f : s->getFields()) {
			f->printGraphViz(file);
			if (++i < s->getFields().size()) 
				file << " | " ;
		}
		file << "\"]\n\n";

		file << "\t" << getNameSpace() << ":"<<s->getID() << " -> " << s->getName() << "\n\n";
	}

	file << "\n";

	if(!prev) {
		file << "}\n";
	}
}