#include <assert.h>
#include <string>
#include <stack>
#include <vector>
#include <algorithm>

#include "astNode.hpp"
#include "symbol.hpp"
#include "symTable.hpp"
#include "varSymNode.hpp"
#include "utypeSymNode.hpp"
#include "tdefSymNode.hpp"
#include "mtypedefSymNode.hpp"

#include <iostream>

/**
 * Split a string into a vector of strings.
*/

std::vector<std::string> split(const std::string &s)
{
	std::vector<std::string> result;
	size_t start = 0;
	size_t end = s.find(" ");
	while (end != std::string::npos)
	{
		result.push_back(s.substr(start, end - start));
		start = end + std::string(" ").length();
		end = s.find(" ", start);
	}
	result.push_back(s.substr(start, end));
	return result;
}

/**
 * Compute the intersection of two vectors of strings.
 
*/

std::vector<std::string> intersection(const std::vector<std::string> &v1, const std::vector<std::string> &v2)
{
	std::vector<std::string> v3;

	std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(v3));

	return v3;
}


/**
 * Compute the Jaccard distance between two vectors of strings.
*/

float jaccard_distance(const std::vector<std::string> &v1, const std::vector<std::string> &v2)
{
	std::vector<std::string> v3 = intersection(v1, v2);

	float jaccard = (float)v3.size() / (float)(v1.size() + v2.size() - v3.size());

	return jaccard;
}

/**
 * Just creates a node with the given values.
 */
astNode::astNode(Type type, int lineNb)
	: type(type)
	, lineNb(lineNb)
	, mId(0)
{}

/**
 * destructor
 */

astNode::~astNode() {
	for(auto n : children)
		delete n.second;
}

/**
 * copy all the children of the given node
*/
void astNode::copyChildren(const astNode& node){
	children.clear();
	for(auto n : node.children)
		children[n.first] = n.second->deepCopy();
}

astNode::Type astNode::getType(void) const {
	return type;
}

int astNode::getLineNb(void) const {
	return lineNb;
}

void astNode::addChild(const std::string& name, astNode* child) {
	if(child) {
		children[name] = child;
	}
}

/*void astNode::moveChild(const std::string& node, astNode* child) {
	detachChild(node);
	addChild(node, child);
}*/

void astNode::deleteChild(const std::string& node) {
	auto res = detachChild(node);
	if (res) delete res;
}

void astNode::eraseChild(const std::string& node, astNode* child) {
	deleteChild(node);
	addChild(node, child);
}

astNode* astNode::detachChild(const std::string& node) {
	auto child = getChild(node);
	if(child) {
		children.erase(node);
	}
	return child;
}

astNode* astNode::getChild(const std::string& node) const {
	auto it = children.find(node);
	if (it == children.end()) 
		return nullptr;
	return it->second;
}

std::list<astNode*> astNode::getChildren(void) const {
	std::list<astNode*> res;
	for(auto c : children)
		res.push_back(c.second);
	return res;
}

/*bool astNode::operator==(const astNode* other) const {
	if(type != other->type)
		return false;
	for(auto child : children) {
		auto child2 = other->children.find(child.first);
		if(child2 == other->children.end())
			return false;
		if(!(*child.second == child2->second))
			return false;
	}
	return true;
}*/

bool astNode::operator==(const astNode* other) const {
	if(type != other->type)
		return false;
	return jaccard_distance(split(std::string(*this)), split(std::string(*other))) == 1;
}

float astNode::similarity(const astNode* other) const {
	auto s = split(std::string(*this));
	auto o = split(std::string(*other));
	
	return jaccard_distance(s, o);
}

std::vector<astNode*> astNode::getMutations(void) const{
	assert(false);
	return std::vector<astNode*>();
}

unsigned int astNode::assignMutables(const Mask& mask, unsigned int id) {
	if(mask.isPresent(type)) {
		if(children.empty() && hasMutations())
			mId = ++id;
		for(auto c : children)
			id = c.second->assignMutables(mask, id);
	}
	return id;
}

bool astNode::mutateMutable(unsigned int id) {
	for(auto n : children) {
		if(n.second->getMId() == id){
			auto mutations = n.second->getMutations();
			assert(mutations.size());
			size_t i = rand() % mutations.size();

			//std::cout << std::string(*mutations[i]) << "\n";

			eraseChild(n.first, mutations[i]);
			mutations.erase(mutations.begin() + i);
			for(auto i : mutations) delete i;
			return true;
		}
	}
	return false;
}

unsigned int astNode::getMId(void) const {
	return mId;
}

bool astNode::hasMutations() const {
    auto mutations = getMutations();
    for (auto it : mutations) {
        delete it;
    }
    return !mutations.empty();
}

int astNode::tab_lvl = 0;

std::string astNode::_tab(int adjust) {
	std::string res = "";
	for(int i = 0; i < tab_lvl + adjust; ++i)
		res += "\t";
	return res;
}

astNode* astNode::mutate(astNode* ast, unsigned int id) {
	std::stack<astNode*> stack;
	stack.push(ast);

	while(!stack.empty()) {
		astNode* node = stack.top();
		stack.pop();
		if(node->mutateMutable(id))
			return ast;
		else
			for(auto c : node->children)
				stack.push(c.second);
	}
	return nullptr;
}

void astNode::acceptVisitor(ASTConstVisitor* visitor) const {
	//visitor = visitor;
	assert(false);
}

void astNode::acceptVisitor(ASTVisitor* visitor) {
	//visitor = visitor;
	assert(false);
}