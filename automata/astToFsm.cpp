#include "astToFsm.hpp"

#include "symbols.hpp"
#include "ast.hpp"
#include "automata.hpp"

#include "tvl.hpp"
#include "expToADD.hpp"

ASTtoFSM::ASTtoFSM() 
    : flags(0)
    , res(nullptr)
    , init(nullptr)
    , current(nullptr)
    , newNode(nullptr)
    , prev(nullptr)
    , fm(nullptr)
    , hasOptFeatures(false)
{}

ASTtoFSM::~ASTtoFSM() {

}

fsm* ASTtoFSM::astToFsm(const symTable* symTab, const stmnt* program, const TVL* fm) {
    
    res = new fsm(symTab, fm->getFeatureModelClauses());
    
    this->fm = fm;
    
    program->acceptVisitor(this);
    //assert(current.size() == 1);
    return res;
}

void ASTtoFSM::_connect(std::list<fsmEdge*>& edges, fsmNode* target){
    for(auto edge : edges) {
        assert(edge->getTargetNode() == nullptr);
        edge->setTargetNode(target);
    }
    edges.clear();
}

void ASTtoFSM::_label(fsmNode* node){
    for(auto label : labels)
        labeledNodes[label] = node;
    labels.clear();
}

fsmEdge* ASTtoFSM::_looseEnd(const stmnt* node) {
    auto edge = current->createfsmEdge(node->getLineNb(), node);

    edge->setFeatures(looseFeatures);
    //fm->printBool(looseFeatures);
    looseFeatures = ADD();

    looseEnds.push_back(edge);
    return edge;
}

fsmEdge* ASTtoFSM::_looseBreak(const stmnt* node) {
    auto edge = current->createfsmEdge(node->getLineNb(), node);
    looseBreaks.push_back(edge);
    return edge;
}

void ASTtoFSM::_toFsm(const stmnt* node) {

    current = (res->createFsmNode(flags, node->getLineNb()));

    if(!init) init = current;

    _label(current);
    
    _connect(looseEnds, current);

    _looseEnd(node);


    node = node->getNext();
    if(node) {
        node->acceptVisitor(this);
    }
}

void ASTtoFSM::visit(const stmntExpr* node)  {

    auto toADD = new expToADD(fm);

    node->getChild()->acceptVisitor(toADD);
    
    if(toADD->isFeatureExpr()){
        
        looseFeatures = toADD->getFormula();
        optFeatures.top() &= ~looseFeatures;
        hasOptFeatures = true;

    } else {

        current = (res->createFsmNode(flags, node->getLineNb()));

        if(!init) init = current;

        _label(current);
        
        _connect(looseEnds, current);

        _looseEnd(node);
        
    }

    delete toADD;

    auto next = node->getNext();
    if(next) {
        next->acceptVisitor(this);
    }
}

void ASTtoFSM::visit(const stmntOpt* node)  {

    node->getBlock()->acceptVisitor(this);
    auto next = node->getNext();
    if(next) {
        next->acceptVisitor(this);
    }
}

void ASTtoFSM::visit(const stmntIf* node)  {
    
    std::list<fsmEdge*> flowLooseEnds;
    std::list<fsmEdge*> flowLooseBreaks;
    
    auto start = res->createFsmNode(flags, node->getLineNb());
    
    current = start;
    
    if(!init) init = current;

    _label(current);
    _connect(looseEnds, current);

    optFeatures.push(fm->getMgr()->addOne());

    auto opt = node->getOpts();
    while(opt){
        //opt->getBlock()->getType() == astNode::E_STMNT_ATOMIC? dynamic_cast<stmntAtomic*>(opt->getBlock())->getBlock() : opt->getBlock();
        auto trans = start->createfsmEdge(node->getLineNb(), new stmntExpr(new exprSkip(node->getLineNb()), node->getLineNb()), nullptr, true);
        looseEnds.push_back(trans);
        
        opt->acceptVisitor(this);

        //(nx, ex, nx+1), (nx+1, ex+1, 0) becomes (nx, ex+1, 0)
        assert(trans->getTargetNode());
        for(auto t : trans->getTargetNode()->getEdges()) {
            t->setSourceNode(start);
        }
        res->deleteNode(trans->getTargetNode());
        
        flowLooseEnds.merge(looseEnds);
        looseEnds.clear();

        opt = opt->getNextOpt();
    }

    looseEnds.merge(flowLooseEnds);

    optFeatures.pop();

    auto next = node->getNext();
    if(next) {
        next->acceptVisitor(this);
    }
}

void ASTtoFSM::visit(const stmntDo* node)  {
    
    std::list<fsmEdge*> flowLooseEnds;
    std::list<fsmEdge*> flowLooseBreaks;

    auto start = res->createFsmNode(flags, node->getLineNb());
    current = start;
    if(!init) init = current;


    _label(current);
    _connect(looseEnds, start);

    optFeatures.push(fm->getMgr()->addOne());

    auto opt = node->getOpts();
    while(opt){
        //auto block = opt->getBlock()->getType() == astNode::E_STMNT_ATOMIC? dynamic_cast<stmntAtomic*>(opt->getBlock())->getBlock() : opt->getBlock();
        auto trans = start->createfsmEdge(node->getLineNb(), new stmntExpr(new exprSkip(node->getLineNb()), node->getLineNb()), nullptr, true);
        looseEnds.push_back(trans);
        
        opt->acceptVisitor(this);

        assert(trans->getTargetNode());
        for(auto t : trans->getTargetNode()->getEdges()) {
            t->setSourceNode(start);
        }
        res->deleteNode(trans->getTargetNode());
        
        
        flowLooseEnds.merge(looseEnds);
        looseEnds.clear();
        flowLooseBreaks.merge(looseBreaks);
        looseBreaks.clear();

        opt = opt->getNextOpt();
    }

    _connect(flowLooseEnds, start);
    looseEnds.merge(flowLooseBreaks);

    optFeatures.pop();

    auto next = node->getNext();
    if(next) {
        next->acceptVisitor(this);
    }
}

void ASTtoFSM::visit(const stmntBreak* node)  {
    

    current = res->createFsmNode(flags, node->getLineNb());

    if(!init) init = current;
    
    _label(current);

    _connect(looseEnds, current);

    _looseBreak(node);


    auto next = node->getNext();
    if(next) {
        next->acceptVisitor(this);
    }
}

void ASTtoFSM::visit(const stmntGoto* node)  {

    current = (res->createFsmNode(flags, node->getLineNb()));

    if(!init) init = current;

    _connect(looseEnds, current);

    looseGotos[node->getLabel()].push_back(current->createfsmEdge(node->getLineNb(), node));

    auto next = node->getNext();
    if(next) {
        next->acceptVisitor(this);
    }
}

void ASTtoFSM::visit(const stmntLabel* node)  {
    std::string label = node->getLabel();
    
    if(label.find("accept") != std::string::npos) 		
        flags |= fsmNode::N_ACCEPT;
    if(label.find("end") != std::string::npos) 			
        flags |= fsmNode::N_END;
    if(label.find("progress") != std::string::npos) 
        flags |= fsmNode::N_PROGRESS;

    labels.push_back(label);

    node->getLabelled()->acceptVisitor(this);

    auto next = node->getNext();
    if(next) {
        next->acceptVisitor(this);
    }
}

void ASTtoFSM::visit(const stmntFct* decl)  {
    init = nullptr;
    auto curStmnt = decl->getBlock();
    curStmnt->acceptVisitor(this);
    res->addInitNode(decl->getFctName(), init);

    for(auto looseGotoList : looseGotos){
        auto labelledNodeIt = labeledNodes.find(looseGotoList.first);
        assert(labelledNodeIt != labeledNodes.end());
        auto labelledNode = labelledNodeIt->second;
        for(auto looseGoto : looseGotoList.second) {
            for(auto inputs : looseGoto->getSourceNode()->getInputEdges()) {
                inputs->setTargetNode(labelledNode);
            }
            res->deleteNode(looseGoto->getSourceNode());
        }
    }
    looseGotos.clear();

    //assert(looseEnds.size() == 1);
    assert(looseGotos.size() == 0);
    assert(looseBreaks.size() == 0);
    looseEnds.clear();
    looseBreaks.clear();
    looseGotos.clear();


    auto next = decl->getNext();
    if(next) {
        next->acceptVisitor(this);
    }
}

void ASTtoFSM::visit(const varDecl* node) {
    auto next = node->getNext();
    if(next) {
        next->acceptVisitor(this);
    }
}

void ASTtoFSM::visit(const chanDecl* node) {
    auto next = node->getNext();
    if(next) {
        next->acceptVisitor(this);
    }
}

void ASTtoFSM::visit(const tdefDecl* node) {
    auto next = node->getNext();
    if(next) {
        next->acceptVisitor(this);
    }
}

void ASTtoFSM::visit(const mtypeDecl* node) {
    auto next = node->getNext();
    if(next) {
        next->acceptVisitor(this);
    }
}

void ASTtoFSM::visit(const stmntAtomic* node)  {
    flags |= fsmNode::N_ATOMIC;
    node->getBlock()->acceptVisitor(this);
    flags &= ~fsmNode::N_ATOMIC;
    auto next = node->getNext();
    if(next)
        next->acceptVisitor(this);
}

void ASTtoFSM::visit(const stmntDStep* node)  {
    flags |= fsmNode::N_DETERMINISTIC;
    node->getBlock()->acceptVisitor(this);
    flags &= ~fsmNode::N_DETERMINISTIC;
    auto next = node->getNext();
    if(next)
        next->acceptVisitor(this);
}

void ASTtoFSM::visit(const stmntElse* node) {


    current = (res->createFsmNode(flags, node->getLineNb()));

    _label(current);
    
    _connect(looseEnds, current);

    if(hasOptFeatures) {
        auto edge = _looseEnd(new stmntExpr(new exprSkip(node->getLineNb()), node->getLineNb()));
        fm->printBool(optFeatures.top());
        edge->setFeatures(optFeatures.top());
        hasOptFeatures = false;
    } 
    else {
        _looseEnd(node);
    }

    auto next = node->getNext();
    if(next) {
        next->acceptVisitor(this);
    }
}

void ASTtoFSM::visit(const stmntChanRecv* node) {
    _toFsm(node);
}

void ASTtoFSM::visit(const stmntChanSnd* node) {
    _toFsm(node);
}

void ASTtoFSM::visit(const stmntAction* node) {
    _toFsm(node);
}

void ASTtoFSM::visit(const stmntAsgn* node) {
    _toFsm(node);
}

void ASTtoFSM::visit(const stmntIncr* node) {
    _toFsm(node);
}

void ASTtoFSM::visit(const stmntDecr* node) {
    _toFsm(node);
}

void ASTtoFSM::visit(const stmntPrint* node) {
    _toFsm(node);
}

void ASTtoFSM::visit(const stmntPrintm* node) {
    _toFsm(node);
}

void ASTtoFSM::visit(const stmntAssert* node) {
    _toFsm(node);
}