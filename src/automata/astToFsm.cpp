#include "astToFsm.hpp"

#include "ast.hpp"
#include "automata.hpp"
#include "symbols.hpp"

#include "expToADD.hpp"
#include "tvl.hpp"

ASTtoFSM::ASTtoFSM()
    : flags(0), res(nullptr), init(nullptr), current(nullptr), newNode(nullptr), prev(nullptr), fm(nullptr),
      hasElseFeatures(false)
{
}

ASTtoFSM::~ASTtoFSM() {}

fsm * ASTtoFSM::astToFsm(const symTable * symTab, const stmnt * program, const TVL * fm)
{
  res = new fsm(symTab, fm? fm->getFeatureModelClauses() : ADD());
  this->fm = fm;

  program->acceptVisitor(this);

  return res;
}

/*
 * Connect all edges without any target node to the target node
 * @param target the target node
 */
void ASTtoFSM::_connect(fsmNode * target)
{
  for (auto edge : looseEnds) {
    assert(edge->getTargetNode() == nullptr);
    edge->setTargetNode(target);
  }
  looseEnds.clear();
}

/*
 * Label all nodes with the current labels
 * @param node the node to label
 */
void ASTtoFSM::_label(fsmNode * node)
{
  for (auto label : labels)
    labeledNodes[label] = node;
  labels.clear();
}

fsmEdge * ASTtoFSM::_looseEnd(const stmnt * node, bool owner)
{
  auto edge = current->createfsmEdge(node->getLineNb(), node, nullptr, owner);

  if (looseFeatures) {
    assert(!looseFeatures.IsOne());
    edge->setFeatures(looseFeatures);
    // fm->printBool(looseFeatures);
    looseFeatures = ADD();
  }

  looseEnds.push_back(edge);
  return edge;
}

fsmEdge * ASTtoFSM::_looseBreak(const stmnt * node)
{
  auto edge = current->createfsmEdge(node->getLineNb(), node);
  looseBreaks.push_back(edge);
  return edge;
}

void ASTtoFSM::_visitNext(const stmnt * node)
{
  auto next = node->getNext();
  if (next)
    next->acceptVisitor(this);
}

/*
 * Transform a statement into a node of the FSM Node
 * @param node the statement to transform
 * @param label if true, label the node with the current labels (default: true)
 * @param connect if true, connect all edges without any target node to the target node (default: true)
 * @param looseEnd if true, add the node to the list of loose ends (default: true)
 * @param looseBreak if true, add the node to the list of loose breaks (default: false)
 */
void ASTtoFSM::_toFsmNode(const stmnt * node, bool label, bool connect, bool looseEnd, bool looseBreak)
{
  current = (res->createFsmNode(flags, node->getLineNb()));
  if (!init)
    init = current;

  if (label)
    _label(current);
  if (connect)
    _connect(current);
  if (looseEnd)
    _looseEnd(node);
  if (looseBreak)
    _looseBreak(node);
}

/*
 * Transform a statement into a node of the FSM (and its children)
 * @param node the statement to transform
 * @param label if true, label the node with the current labels (default: true)
 * @param connect if true, connect all edges without any target node to the target node (default: true)
 * @param looseEnd if true, add the node to the list of loose ends (default: true)
 * @param looseBreak if true, add the node to the list of loose breaks (default: false)
 */
void ASTtoFSM::_toFsm(const stmnt * node, bool label, bool connect, bool looseEnd, bool looseBreak)
{
  _toFsmNode(node, label, connect, looseEnd, looseBreak);
  _visitNext(node);
}

void ASTtoFSM::visit(const stmntExpr * node)
{

  if(fm) {
    auto toADD = expToADD(fm);

    node->getChild()->acceptVisitor(&toADD);

    if (toADD.isFeatureExpr()) {
      looseFeatures = toADD.getFormula();
      assert(!looseFeatures || !(looseFeatures.IsOne()));
      elseFeatures &= ~looseFeatures;
      hasElseFeatures = true;
      _visitNext(node);
    }
    else {
      _toFsm(node);
    }

  } else {
    _toFsm(node);
  }
}

void ASTtoFSM::visit(const stmntOpt * node) { node->getBlock()->acceptVisitor(this); }

void ASTtoFSM::visit(const stmntIf * node)
{

  std::list<fsmEdge *> flowLooseEnds;
  std::list<fsmEdge *> flowLooseBreaks;
  _toFsmNode(node, true, true, false, false);
  auto start = current;
  //

  // if looseFeatures then the if is a child of a featured if (if:: F :: if :: b == true ...)
  // else a simple if or a featured if
  /*if(looseFeatures)
      _looseEnd(new stmntExpr(new exprSkip(node->getLineNb()), node->getLineNb()), true);
    */

  auto looseFeaturesSave = looseFeatures;
  assert(!looseFeatures || !(looseFeatures.IsOne()));
  looseFeatures = ADD();

  auto elseFeaturesSave = elseFeatures;
  elseFeatures = fm? fm->getMgr()->addOne() : ADD();

  auto hasElseFeaturesSave = hasElseFeatures;
  hasElseFeatures = false;

  auto opt = node->getOpts();
  while (opt) {
    // opt->getBlock()->getType() == astNode::E_STMNT_ATOMIC? dynamic_cast<stmntAtomic*>(opt->getBlock())->getBlock() :
    // opt->getBlock();

    auto trans = start->createfsmEdge(node->getLineNb(), new stmntExpr(new exprSkip(node->getLineNb()), node->getLineNb()),
                                      nullptr, true);
    looseEnds.push_back(trans);

    // assert(!looseFeatures);
    // looseFeatures = looseFeaturesSave;
    //_looseEnd(new stmntExpr(new exprSkip(node->getLineNb()), node->getLineNb()), true);
    /*if(looseFeaturesSave)
        trans->setFeatures(looseFeaturesSave);*/

    opt->acceptVisitor(this);

    if (looseFeatures) {
      trans->setFeatures(looseFeatures);
      looseFeatures = ADD();
    }

    //(nx, ex, nx+1), (nx+1, ex+1, 0) becomes (nx, ex+1, 0)
    if (!trans->getFeatures()) {
      assert(trans->getTargetNode());
      for (auto t : trans->getTargetNode()->getEdges()) {
        t->setSourceNode(start);
      }
      res->deleteNode(trans->getTargetNode());
    }

    // if(!looseFeatures){
    //(nx, ex, nx+1), (nx+1, ex+1, 0) becomes (nx, ex+1, 0)
    // assert(trans->getTargetNode());
    // for(auto t : trans->getTargetNode()->getEdges()) {
    //     t->setSourceNode(start);
    // }
    // assert(trans->getTargetNode()->getEdges().size() == 1);
    // res->deleteNode(trans->getTargetNode());

    //} else {
    // trans->setFeatures(looseFeatures);
    //}

    flowLooseEnds.merge(looseEnds);
    looseEnds.clear();

    opt = opt->getNextOpt();
  }

  // if the if is featured, else features have been built, just apply
  if (hasElseFeatures) {
    // assert(!looseFeatures);
    for (auto edge : start->getEdges()) {

      if (edge->getExpression()->getType() == astNode::E_STMNT_ELSE) {

        edge->setExpression(new stmntExpr(new exprSkip(node->getLineNb()), node->getLineNb()));
        edge->owner = true;
        edge->setFeatures(elseFeatures);

        // res->skip(tskip->getTargetNode());
      }
    }
  }

  /******************************/

  looseEnds.merge(flowLooseEnds);

  _visitNext(node);

  looseFeatures = looseFeaturesSave;
  elseFeatures = elseFeaturesSave;
  hasElseFeatures = hasElseFeaturesSave;
}

void ASTtoFSM::visit(const stmntDo * node)
{

  std::list<fsmEdge *> flowLooseEnds;
  std::list<fsmEdge *> flowLooseBreaks;

  _toFsmNode(node, true, true, false, false);
  auto start = current;

  // if looseFeatures then the if is a child of a featured if (if:: F :: if :: b == true ...)
  // else a simple if or a featured if
  // auto looseFeaturesSave = looseFeatures;
  // assert(!looseFeatures || !looseFeatures.IsOne());
  // looseFeatures = ADD();

  // auto elseFeaturesSave = elseFeatures;
  // elseFeatures = fm->getMgr()->addOne();

  // auto hasElseFeaturesSave = hasElseFeatures;
  // hasElseFeatures = false;

  auto opt = node->getOpts();
  while (opt) {
    // auto block = opt->getBlock()->getType() == astNode::E_STMNT_ATOMIC?
    // dynamic_cast<stmntAtomic*>(opt->getBlock())->getBlock() : opt->getBlock();
    auto trans = start->createfsmEdge(node->getLineNb(), new stmntExpr(new exprSkip(node->getLineNb()), node->getLineNb()),
                                      nullptr, true);
    looseEnds.push_back(trans);

    opt->acceptVisitor(this);

    assert(!looseFeatures);

    assert(trans->getTargetNode());
    for (auto t : trans->getTargetNode()->getEdges()) {
      t->setSourceNode(start);
    }
    res->deleteNode(trans->getTargetNode());

    flowLooseEnds.merge(looseEnds);
    looseEnds.clear();
    flowLooseBreaks.merge(looseBreaks);
    looseBreaks.clear();

    opt = opt->getNextOpt();
  }

  assert(looseEnds.size() == 0);
  looseEnds = flowLooseEnds;
  _connect(start);

  // looseFeatures = looseFeaturesSave;
  // elseFeatures = elseFeaturesSave;
  // hasElseFeatures = hasElseFeaturesSave;

  /******************************/

  looseEnds.merge(flowLooseBreaks);

  _visitNext(node);
}

void ASTtoFSM::visit(const stmntSeq * node) { node->getBlock()->acceptVisitor(this); }

void ASTtoFSM::visit(const stmntBreak * node) { _toFsm(node, true, true, false, true); }

void ASTtoFSM::visit(const stmntGoto * node)
{

  _toFsmNode(node, false, true, false, false);

  looseGotos[node->getLabel()].push_back(current->createfsmEdge(node->getLineNb(), node));

  _visitNext(node);
}

void ASTtoFSM::visit(const stmntLabel * node)
{
  std::string label = node->getLabel();

  if (label.find("accept") != std::string::npos)
    flags |= fsmNode::N_ACCEPT;
  if (label.find("end") != std::string::npos)
    flags |= fsmNode::N_END;
  if (label.find("progress") != std::string::npos)
    flags |= fsmNode::N_PROGRESS;

  labels.push_back(label);

  node->getLabelled()->acceptVisitor(this);

  // BUGGY (e.g., for atomic...)
  flags = 0;

  _visitNext(node);
}

void ASTtoFSM::visit(const stmntFct * decl)
{
  init = nullptr;
  auto curStmnt = decl->getBlock();
  curStmnt->acceptVisitor(this);
  res->addInitNode(decl->getFctName(), init);

  for (auto looseGotoList : looseGotos) {
    auto labelledNodeIt = labeledNodes.find(looseGotoList.first);
    assert(labelledNodeIt != labeledNodes.end());
    auto labelledNode = labelledNodeIt->second;
    for (auto looseGoto : looseGotoList.second) {
      for (auto inputs : looseGoto->getSourceNode()->getInputEdges()) {
        inputs->setTargetNode(labelledNode);
      }
      res->deleteNode(looseGoto->getSourceNode());
    }
  }
  looseGotos.clear();

  // assert(looseEnds.size() == 1);
  assert(looseGotos.size() == 0);
  assert(looseBreaks.size() == 0);
  looseEnds.clear();
  looseBreaks.clear();
  looseGotos.clear();

  auto next = decl->getNext();
  if (next) {
    next->acceptVisitor(this);
  }
}

void ASTtoFSM::visit(const varDecl * node) { _visitNext(node); }

void ASTtoFSM::visit(const chanDecl * node) { _visitNext(node); }

void ASTtoFSM::visit(const tdefDecl * node) { _visitNext(node); }

void ASTtoFSM::visit(const mtypeDecl * node) { _visitNext(node); }

void ASTtoFSM::visit(const stmntAtomic * node)
{
  flags |= fsmNode::N_ATOMIC;
  node->getBlock()->acceptVisitor(this);
  flags &= ~fsmNode::N_ATOMIC;
  _visitNext(node);
}

void ASTtoFSM::visit(const stmntDStep * node)
{
  flags |= fsmNode::N_DETERMINISTIC;
  node->getBlock()->acceptVisitor(this);
  flags &= ~fsmNode::N_DETERMINISTIC;
  _visitNext(node);
}

void ASTtoFSM::visit(const stmntElse * node)
{
  _toFsm(node);
  //   if (hasElseFeatures) {
  //     // auto edge = _looseEnd(new stmntExpr(new exprSkip(node->getLineNb()), node->getLineNb()), true);
  //     // edge->setFeatures(elseFeatures);
  //     // hasElseFeatures = false;
  //     _looseEnd(node);
  //   }
  //   else
  //     _looseEnd(node);
}

void ASTtoFSM::visit(const stmntChanRecv * node) { _toFsm(node); }

void ASTtoFSM::visit(const stmntChanSnd * node) { _toFsm(node); }

void ASTtoFSM::visit(const stmntAction * node) { _toFsm(node); }

void ASTtoFSM::visit(const stmntAsgn * node) { _toFsm(node); }

void ASTtoFSM::visit(const stmntIncr * node) { _toFsm(node); }

void ASTtoFSM::visit(const stmntDecr * node) { _toFsm(node); }

void ASTtoFSM::visit(const stmntPrint * node) { _toFsm(node); }

void ASTtoFSM::visit(const stmntPrintm * node) { _toFsm(node); }

void ASTtoFSM::visit(const stmntAssert * node) { _toFsm(node); }