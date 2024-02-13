#include <algorithm>
#include <assert.h>
#include <iostream>
#include <stack>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "fsm.hpp"
#include "fsmEdge.hpp"
#include "fsmNode.hpp"

#include "ast.hpp"
#include "symbols.hpp"

#include "tvl.hpp"

/*
 * FINITE STATE MACHINES (FSMs)
 * * * * * * * * * * * * * * * * * * * * * * * */

fsm::fsm(const symTable * globalSymTab, const ADD & fd) : globalSymTab(nullptr), sysSymTab(nullptr), fd(fd)
{
  assert(globalSymTab->getNameSpace() == "global" || globalSymTab->getNameSpace() == "system");
  this->globalSymTab = globalSymTab->getNameSpace() == "global" ? globalSymTab : globalSymTab->getSubSymTab("global");
  this->sysSymTab = globalSymTab->getNameSpace() == "system" ? globalSymTab : globalSymTab->prevSymTab();
}

/**
 * Destroys an FSM and all that's linked to it.
 */
fsm::~fsm()
{
  for (fsmNode * node : nodes)
    delete node;
  for (fsmEdge * trans : trans)
    delete trans;
}

const symTable * fsm::getGlobalSymTab(void) const { return globalSymTab; }

const symTable * fsm::getSystemSymTab(void) const { return sysSymTab; }

/**
 * Creates a node and adds it to the node list of the fsm.
 * The node has to be manually attached to a transition.
 */

fsmNode * fsm::createFsmNode(int flags, int lineNb)
{
  fsmNode * node = new fsmNode(flags, lineNb, this);
  nodes.push_back(node);
  return node;
}

/**
 * Copies and FSM node and add it to the node list of an FSM.
 */
fsmNode * fsm::copyFsmNode(const fsmNode * node) { return createFsmNode(node->getFlags(), node->getLineNb()); }

fsmNode * fsm::getNode(unsigned int lineNb) const
{
  for (auto node : nodes) {
    unsigned int nodeLineNb = node->getLineNb();
    if (nodeLineNb == lineNb)
      return node;
  }
  return nullptr;
}

std::list<fsmNode *> fsm::getNodes(void) const { return nodes; }

std::list<fsmNode *> fsm::getNodes(fsmNode * from) const
{
  std::list<fsmNode *> reachables;
  std::stack<fsmNode *> stack;

  stack.push(from);

  while (!stack.empty()) {
    auto current = stack.top();
    stack.pop();

    if (std::find(reachables.begin(), reachables.end(), current) == reachables.end())
      reachables.push_back(current);

    for (auto e : current->getEdges()) {
      auto next = e->getTargetNode();
      if (next && std::find(reachables.begin(), reachables.end(), next) == reachables.end()) {
        stack.push(next);
      }
    }
  }

  return reachables;
}

void fsm::deleteTransition(fsmEdge * edge)
{
  assert(std::find(this->trans.begin(), this->trans.end(), edge) != this->trans.end());

  auto source = edge->getSourceNode();
  if (source)
    source->removeTransition(edge);
  auto dest = edge->getTargetNode();
  if (dest)
    dest->removeInputTransition(edge);

  trans.remove(edge);
  delete edge;
}

void fsm::orderAcceptingTransitions(void)
{
  for (auto node : nodes)
    node->orderAcceptingTransitions();
}

/** TODO: not done yet
 * Projects the FSM onto a set of features.
 * The resulting FSM is a copy of the original FSM where all featured transitions not in the feature set are removed.
 * @param vars: The set of features to project onto.
 */
fsm * fsm::project(const std::list<std::string> & features) const
{
  // Create a new FSM
  auto newFsm = new fsm(globalSymTab, fd);
  std::map<fsmNode *, fsmNode *> nodeMap; // A map from old nodes to new nodes
  std::map<fsmEdge *, fsmEdge *> edgeMap; // A map from old edges to new edges

  // Create a copy of all nodes and add them to the new FSM
  for (auto node : nodes) {
    auto newNode = newFsm->createFsmNode(node->getFlags(), node->getLineNb());
    nodeMap[node] = newNode;
  }

  for (auto node : nodes) {
    for (auto edge : node->getEdges()) {
      if (!edge->hasFeatures()) {
        // No features, just copy the edge
        auto newEdge = new fsmEdge(nodeMap[edge->getSourceNode()], nodeMap[edge->getTargetNode()], edge->getExpression(),
                                   edge->getFeatures(), edge->getProbability(), edge->getLineNb());
        newFsm->addTransition(newEdge);
        edgeMap[edge] = newEdge;
      }
      else {
        // Need to check if the edge if the edge can be taken by one of the features
        auto newEdge = new fsmEdge(nodeMap[edge->getSourceNode()], nodeMap[edge->getTargetNode()], edge->getExpression(),
                                   edge->getFeatures(), edge->getProbability(), edge->getLineNb());
        if (edge->getFeatures()) {
        }
        if (newEdge->hasFeatures()) {
          newFsm->addTransition(newEdge);
          edgeMap[edge] = newEdge;
        }
        else {
          delete newEdge;
        }
      }
    }
  }

  for (auto init : inits) {
    newFsm->addInitNode(init.first, nodeMap[init.second]);
  }

  // Remove all nodes without any transitions
  for (auto node : newFsm->getNodes()) {
    if (node->getEdges().empty() && node->getInputEdges().empty()) {
      newFsm->deleteNode(node);
    }
  }

  // Return the projected FSM
  return newFsm;
}

void fsm::removeUselessTransitions(void)
{
  int featuredT = 0;
  auto tcopy = trans;
  for (auto t : tcopy) {
    if (t->expression->getType() == astNode::E_STMNT_EXPR) {
      auto node = dynamic_cast<const stmntExpr *>(t->expression)->getChild();
      if (node->getType() == astNode::E_EXPR_SKIP && t->features) {
        // if(!t->features) {
        auto src = t->getSourceNode();
        auto target = t->getTargetNode();
        if (target && target->getInputEdges().size() == 1) {
          // assert(!target || target->getInputEdges().size() == 1);
          auto t_feat = t->getFeatures();
          deleteTransition(t);
          if (target && target->getEdges().size()) {
            for (auto nextT : target->getEdges()) {
              nextT->setFeatures(t_feat ? (nextT->getFeatures() ? t->getFeatures() & nextT->getFeatures() : t_feat)
                                        : nextT->getFeatures());
              nextT->setSourceNode(src);
            }
          }
        }
        //} /*else	{
        // features skip
        // featuredT++;
        // TVL::printBool(t->features);
        //}*/
      }
    }
  }
  auto ncopy = nodes;
  for (auto node : ncopy) {

    auto initNode = false;
    for (auto start : inits) {
      if (start.second == node) {
        initNode = true;
        break;
      }
    }

    if (node->getInputEdges().size() == 0 && !initNode) {
      nodes.remove(node);
      delete node;
    }
  }
}

void fsm::skip(fsmNode * toSkip)
{
  assert(toSkip->getInputEdges().size() == 1);
  auto inputEdge = *toSkip->getInputEdges().begin();
  assert(inputEdge->getExpression()->getType() == astNode::E_STMNT_EXPR);
  auto node = dynamic_cast<const stmntExpr *>(inputEdge->getExpression())->getChild();
  assert(node->getType() == astNode::E_EXPR_SKIP);

  auto newSrc = inputEdge->getSourceNode();
  for (auto newSrcTs : newSrc->getEdges()) {
    assert(dynamic_cast<const stmntExpr *>(newSrcTs->getExpression())->getChild()->getType() == astNode::E_EXPR_SKIP);
  }

  deleteTransition(inputEdge);

  assert(newSrc);
  for (auto outputs : toSkip->getEdges()) {
    outputs->setSourceNode(newSrc);
  }
}

void fsm::deleteNode(fsmNode * node)
{
  // Assert that the node is in the fsm
  assert(std::find(this->nodes.begin(), this->nodes.end(), node) != this->nodes.end());

  // Delete all transitions that are connected to the node
  for (auto t : node->getEdges())
    deleteTransition(t);
  for (auto t : node->getInputEdges())
    deleteTransition(t);

  nodes.remove(node);
  delete node;
}

void fsm::addInitNode(const std::string & procName, fsmNode * node) { inits[procName] = node; }

std::map<std::string, fsmNode *> fsm::getInitNodes() const { return inits; }

fsmNode * fsm::getFsmWithName(const std::string & name) const { return inits.at(name); }

std::list<fsmEdge *> fsm::getTransitions(void) const { return trans; }

std::list<fsmEdge *> fsm::findTransitions(fsmNode * from, std::function<bool(fsmEdge *)> edgePredicate) const
{
  std::list<fsmNode *> reachables;
  std::set<fsmEdge *> edges;
  std::stack<fsmNode *> stack;

  stack.push(from);

  while (!stack.empty()) {
    auto current = stack.top();
    stack.pop();

    // Do not add the same node twice
    if (std::find(reachables.begin(), reachables.end(), current) == reachables.end())
      reachables.push_back(current);

    for (auto e : current->getEdges()) {
      if (edgePredicate(e)) {
        edges.insert(e);
      }
      auto next = e->getTargetNode();
      if (next && std::find(reachables.begin(), reachables.end(), next) == reachables.end()) {
        stack.push(next);
      }
    }
  }

  return std::list<fsmEdge *>(edges.begin(), edges.end());
}

std::list<fsmEdge *> fsm::getTransitions(fsmNode * from) const
{
  // Define a predicate function for collecting all edges
  auto allEdgesPredicate = [](fsmEdge * e) { return true; };
  return findTransitions(from, allEdgesPredicate);
}

std::list<fsmEdge *> fsm::getEndTransitions(fsmNode * from) const
{
  // Define a predicate function for collecting end edges
  auto endEdgesPredicate = [](fsmEdge * e) { return e->getTargetNode() == nullptr; };
  return findTransitions(from, endEdgesPredicate);
}

std::list<fsmEdge *> fsm::getEndTransitions(void) const
{
  std::list<fsmEdge *> res;
  for (auto t : trans)
    if (t->getTargetNode() == nullptr)
      res.push_back(t);
  return res;
}

void fsm::addTransition(fsmEdge * edge) { trans.push_back(edge); }

bool fsm::isFeatured(void) const { return fd; }

const ADD & fsm::getFeatureDiagram(void) const { return fd; }

void fsm::printGraphVis(std::ofstream & file) const
{
  file << "digraph finite_state_machine {\n";
  file << "\trankdir=LR\n";
  file << "\tsize=\"8,5\"\n";

  for (auto init : inits) {
    file << "\t" << init.second->getID() << " [label = " << init.second->getLineNb()
         << ", shape = doublecircle, fixedsize = true]; \n";
    file << "\ts" << init.second->getID() << " [shape = point];\n";
  }

  for (auto end : getEndTransitions())
    file << "\te" << end->getSourceNode()->getID()
         << " [shape = doublecircle, fixedsize = true, style = filled, fillcolor = black, fontcolor = white, label = end];\n";

  for (auto node : nodes) {
    if (!node->getInputEdges().empty())
      file << "\t " << node->getID() << " [label = " << node->getLineNb() << ", shape = circle, fixedsize = true "
           << ((node->getFlags() & fsmNode::N_ATOMIC) ? ", style = dotted" : "") << "];\n";
  }

  for (auto init : inits)
    file << "\ts" << init.second->getID() << " -> " << init.second->getID() << ";\n";

  for (auto t : trans) {
    auto exprStr = std::string(*t->getExpression());
    std::replace(exprStr.begin(), exprStr.end(), '\"', ' ');
    std::replace(exprStr.begin(), exprStr.end(), '\n', ' ');
    if (t->getTargetNode()) {
      file << "\t" << t->getSourceNode()->getID() << " -> " << t->getTargetNode()->getID() << " [label = \""
           << (t->getProbability() != 1.0 ? " [" + std::to_string(t->getProbability()) + "] " : "") << exprStr << "\"];\n";
    }
    else
      file << "\t" << t->getSourceNode()->getID() << " -> e" << t->getSourceNode()->getID() << " [label = \""
           << (t->getProbability() != 1.0 ? " [" + std::to_string(t->getProbability()) + "] " : "") << exprStr << "\"];\n";
  }

  file << "}";
}

void fsm::printGraphVisWithLocations(std::ofstream & file, const std::list<const fsmNode *> & locs,
                                     const std::list<const fsmEdge *> & edges) const
{
  file.precision(3);

  file << "digraph finite_state_machine {\n";
  file << "\trankdir=LR\n";
  file << "\tsize=\"8,5\"\n";

  for (auto init : inits) {
    file << "\t" << init.second->getID() << " [label = " << init.second->getLineNb() << ", shape = doublecircle, "
         << (std::find(locs.begin(), locs.end(), init.second) != locs.end() ? "color = red, " : "") << "fixedsize = true]; \n";
    file << "\ts" << init.second->getID() << " [shape = point];\n";
  }

  for (auto end : getEndTransitions())
    file << "\te" << end->getSourceNode()->getID() << " [shape = doublecircle, fixedsize = true, style = filled, "
         << (std::find(locs.begin(), locs.end(), end->getSourceNode()) != locs.end() ? "color = red, "
                                                                                     : "fillcolor = black, fontcolor = white, ")
         << " label = end];\n";

  for (auto node : nodes) {
    if (!node->getInputEdges().empty())
      file << "\t " << node->getID() << " [label = " << node->getLineNb() << ", shape = circle, "
           << (std::find(locs.begin(), locs.end(), node) != locs.end() ? "color = red, " : "") << "fixedsize = true "
           << ((node->getFlags() & fsmNode::N_ATOMIC) ? ", style = dotted" : "") << "];\n";
  }

  for (auto init : inits)
    file << "\ts" << init.second->getID() << " -> " << init.second->getID() << ";\n";

  for (auto t : trans) {
    auto exprStr = std::string(*t->getExpression());
    std::replace(exprStr.begin(), exprStr.end(), '\"', ' ');
    std::replace(exprStr.begin(), exprStr.end(), '\n', ' ');
    if (t->getTargetNode()) {
      file << "\t" << t->getSourceNode()->getID() << " -> " << t->getTargetNode()->getID() << " ["
           << (std::find(edges.begin(), edges.end(), t) != edges.end() ? "color = red," : "") << " label = \""
           << (t->getProbability() != 1.0 ? " [" + std::to_string(t->getProbability()) + "] " : "") << t->getLineNb() << " | "
           << exprStr << "\"];\n";
    }
    else
      file << "\t" << t->getSourceNode()->getID() << " -> e" << t->getSourceNode()->getID() << " ["
           << (std::find(edges.begin(), edges.end(), t) != edges.end() ? "color = red," : "") << " label = \""
           << (t->getProbability() != 1.0 ? " [" + std::to_string(t->getProbability()) + "] " : "") << t->getLineNb() << " | "
           << exprStr << "\"];\n";
  }
  file << "}";
}