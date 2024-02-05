#include <algorithm>
#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "ast.hpp"
#include "fsm.hpp"
#include "fsmEdge.hpp"
#include "fsmNode.hpp"
#include "symbols.hpp"
#include "tvl.hpp"

fsmEdge::fsmEdge(fsmNode * source, const astNode * expression, int lineNb, bool owner)
    : parent(source->getParent()), source(source), target(nullptr), expression(expression), lineNb(lineNb), prob(1.0),
      owner(owner)
{
  auto stmntCast = dynamic_cast<const stmnt *>(expression);
  prob = stmntCast ? stmntCast->getProb() : prob;

  // std::cout << "add (n"<< source->getLineNb() << ", e"<< lineNb << ", n-1)" << std::endl;
}

fsmEdge::fsmEdge(fsmNode *source, fsmNode *target, const astNode *expression, int lineNb, bool owner)
	: parent(source->getParent()), source(source), target(target), expression(expression), lineNb(lineNb), prob(1.0), owner(owner)
{
  auto stmntCast = dynamic_cast<const stmnt *>(expression);
  prob = stmntCast ? stmntCast->getProb() : prob;
}

fsmEdge::fsmEdge(fsmNode *source, fsmNode *target, const astNode *expression, ADD features, double prob, int lineNb, bool owner)
	: parent(source->getParent()), source(source), target(target), features(features), expression(expression), lineNb(lineNb), prob(prob), owner(owner)
{
  auto stmntCast = dynamic_cast<const stmnt *>(expression);
  prob = stmntCast ? stmntCast->getProb() : prob;
}


/**
 * Destroys a transition; but not the attached nodes.
 */
fsmEdge::~fsmEdge()
{
  if (owner)
    delete expression;
}

fsmNode * fsmEdge::setTargetNode(fsmNode * target)
{
  assert(target);
  if (this->target) {
    this->target->removeInputTransition(this);
  }

  auto old = this->target;
  this->target = target;
  this->target->addInputTransition(this);

  return old;
}

fsmNode * fsmEdge::getTargetNode(void) const { return target; }

fsmNode * fsmEdge::setSourceNode(fsmNode * source)
{
  assert(source);

  if (this->source) {
    this->source->removeTransition(this);
  }

  auto old = this->source;
  this->source = source;
  if (this->source)
    this->source->addTransition(this);

  return old;
}

fsmNode * fsmEdge::getSourceNode(void) const { return source; }

const astNode * fsmEdge::getExpression(void) const { return this->expression; }

void fsmEdge::setExpression(const astNode * expression) { this->expression = expression; }

void fsmEdge::setLineNb(int line) { lineNb = line; }

int fsmEdge::getLineNb(void) const { return lineNb; }

double fsmEdge::getProbability(void) const { return prob; }

fsmEdge::operator std::string(void) const { return expression ? std::string(*expression) : ""; }

bool fsmEdge::operator==(const fsmEdge& other) const { return *expression == other.expression; }

float fsmEdge::similarity(const fsmEdge& other) const { return expression->similarity(other.expression); }

bool fsmEdge::hasFeatures(void) const { return features.getNode() != nullptr; }

const ADD & fsmEdge::getFeatures(void) const { return features; }

void fsmEdge::setFeatures(const ADD & features)
{
  assert(features && !(features.IsOne()));
  this->features = features;
  // std::cout << "setfd ["<< TVL::toString(features) << "](n"<< source->getLineNb() << ", e"<< lineNb << ", n-1)" << std::endl;
}