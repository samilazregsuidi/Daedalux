// #include "fsmToAST.hpp"

// #include "symbols.hpp"
// #include "ast.hpp"
// #include "automata.hpp"

// #include "tvl.hpp"
// #include "expToADD.hpp"

// FSMtoAST::FSMtoAST() 
//     : flags(0)
//     , res(nullptr)
//     , init(nullptr)
//     , current(nullptr)
//     , newNode(nullptr)
//     , prev(nullptr)
//     , fm(nullptr)
//     , hasElseFeatures(false)
// {}

// FSMtoAST::~FSMtoAST() {

// }

// fsm* FSMtoAST::fSMtoAST(const symTable* symTab, const stmnt* program, const TVL* fm) {
    
//     res = new fsm(symTab, fm->getFeatureModelClauses());
    
//     this->fm = fm;
    
//     program->acceptVisitor(this);
//     //assert(current.size() == 1);

//     //res->removeUselessTransitions();

//     return res;
// }

// void FSMtoAST::toAST(fsmNode* node){
//     node.
// }

// void FSMtoAST::_label(fsmNode* node){
//     for(auto label : labels)
//         labeledNodes[label] = node;
//     labels.clear();
// }

// fsmEdge* FSMtoAST::_looseEnd(const stmnt* node, bool owner) {
//     auto edge = current->createfsmEdge(node->getLineNb(), node, nullptr, owner);

//     if(looseFeatures) {
//         assert(!looseFeatures.IsOne());
//         edge->setFeatures(looseFeatures);
//         //fm->printBool(looseFeatures);
//         looseFeatures = ADD();
//     }

//     looseEnds.push_back(edge);
//     return edge;
// }

// fsmEdge* FSMtoAST::_looseBreak(const stmnt* node) {
//     auto edge = current->createfsmEdge(node->getLineNb(), node);
//     looseBreaks.push_back(edge);
//     return edge;
// }

// void FSMtoAST::_toFsm(const stmnt* node) {

//     current = (res->createFsmNode(flags, node->getLineNb()));

//     if(!init) init = current;

//     _label(current);
    
//     _connect(current);

//     _looseEnd(node);


//     node = node->getNext();
//     if(node) {
//         node->acceptVisitor(this);
//     }
// }

// void FSMtoAST::visit(const stmntExpr* node)  {

//     auto toADD = expToADD(fm);

//     node->getChild()->acceptVisitor(&toADD);
    
//     if(toADD.isFeatureExpr()){
        
//         looseFeatures = toADD.getFormula();
//         assert(!looseFeatures || !(looseFeatures.IsOne()));
//         elseFeatures &= ~looseFeatures;
//         hasElseFeatures = true;

//     } else {

//         current = (res->createFsmNode(flags, node->getLineNb()));

//         if(!init) init = current;

//         _label(current);
        
//         _connect(current);

//         _looseEnd(node);
        
//     }

//     auto next = node->getNext();
//     if(next) {
//         next->acceptVisitor(this);
//     }
// }

// void FSMtoAST::visit(const stmntOpt* node)  {

//     node->getBlock()->acceptVisitor(this);
// }

// void FSMtoAST::visit(const stmntIf* node)  {
    
//     std::list<fsmEdge*> flowLooseEnds;
//     std::list<fsmEdge*> flowLooseBreaks;
    
//     auto start = res->createFsmNode(flags, node->getLineNb());
    
//     current = start;
    
//     if(!init) init = current;

//     _label(current);
//     _connect(current);

//     //if looseFeatures then the if is a child of a featured if (if:: F :: if :: b == true ...)
//     //else a simple if or a featured if 
//     /*if(looseFeatures)
//         _looseEnd(new stmntExpr(new exprSkip(node->getLineNb()), node->getLineNb()), true);

//     start = res->createFsmNode(flags, node->getLineNb());
//     current = start;
//     _label(current);
//     _connect(current);*/

    
//     auto looseFeaturesSave = looseFeatures;
//     assert(!looseFeatures || !(looseFeatures.IsOne()));
//     looseFeatures = ADD();

//     auto elseFeaturesSave = elseFeatures;
//     elseFeatures = fm->getMgr()->addOne();

//     auto hasElseFeaturesSave = hasElseFeatures;
//     hasElseFeatures = false;

//     auto opt = node->getOpts();
//     while(opt){
//         //opt->getBlock()->getType() == astNode::E_STMNT_ATOMIC? dynamic_cast<stmntAtomic*>(opt->getBlock())->getBlock() : opt->getBlock();
        
//         auto trans = start->createfsmEdge(node->getLineNb(), new stmntExpr(new exprSkip(node->getLineNb()), node->getLineNb()), nullptr, true);
//         looseEnds.push_back(trans);

//         //assert(!looseFeatures);
//         //looseFeatures = looseFeaturesSave;
//         //_looseEnd(new stmntExpr(new exprSkip(node->getLineNb()), node->getLineNb()), true);
//         /*if(looseFeaturesSave)
//             trans->setFeatures(looseFeaturesSave);*/

//         opt->acceptVisitor(this);

//         if(looseFeatures) {
//             trans->setFeatures(looseFeatures);
//             looseFeatures = ADD();
//         }

//         //(nx, ex, nx+1), (nx+1, ex+1, 0) becomes (nx, ex+1, 0) 
//         if(!trans->getFeatures()) {
//             assert(trans->getTargetNode());
//             for(auto t : trans->getTargetNode()->getEdges()) {
//                 t->setSourceNode(start);
//             }
//             res->deleteNode(trans->getTargetNode());
//         }

//         //if(!looseFeatures){
//             //(nx, ex, nx+1), (nx+1, ex+1, 0) becomes (nx, ex+1, 0) 
//             //assert(trans->getTargetNode());
//             //for(auto t : trans->getTargetNode()->getEdges()) {
//             //    t->setSourceNode(start);
//             //}
//             //assert(trans->getTargetNode()->getEdges().size() == 1);
//             //res->deleteNode(trans->getTargetNode());
        
//         //} else {
//             //trans->setFeatures(looseFeatures);
//         //}

//         flowLooseEnds.merge(looseEnds);
//         looseEnds.clear();

//         opt = opt->getNextOpt();
//     }

//     //if the if is featured, else features have been built, just apply
//     if(hasElseFeatures) {
//         //assert(!looseFeatures);
//         for(auto edge : start->getEdges()) {
            
//             if(edge->getExpression()->getType() == astNode::E_STMNT_ELSE) {
                
//                 edge->setExpression(new stmntExpr(new exprSkip(node->getLineNb()), node->getLineNb()));
//                 edge->owner = true;
//                 edge->setFeatures(elseFeatures);

//                 //res->skip(tskip->getTargetNode());
//             }
//         }
//     }

//     /******************************/

//     looseEnds.merge(flowLooseEnds);

//     auto next = node->getNext();
//     if(next) {
//         next->acceptVisitor(this);
//     }

//     looseFeatures = looseFeaturesSave;
//     elseFeatures = elseFeaturesSave;
//     hasElseFeatures = hasElseFeaturesSave;
// }

// void FSMtoAST::visit(const stmntDo* node)  {
    
//     std::list<fsmEdge*> flowLooseEnds;
//     std::list<fsmEdge*> flowLooseBreaks;

//     auto start = res->createFsmNode(flags, node->getLineNb());
//     current = start;
//     if(!init) init = current;


//     _label(current);
//     _connect(start);

//     //if looseFeatures then the if is a child of a featured if (if:: F :: if :: b == true ...)
//     //else a simple if or a featured if 
//     //auto looseFeaturesSave = looseFeatures;
//     //assert(!looseFeatures || !looseFeatures.IsOne());
//     //looseFeatures = ADD();

//     //auto elseFeaturesSave = elseFeatures;
//     //elseFeatures = fm->getMgr()->addOne();

//     //auto hasElseFeaturesSave = hasElseFeatures;
//     //hasElseFeatures = false;

//     auto opt = node->getOpts();
//     while(opt){
//         //auto block = opt->getBlock()->getType() == astNode::E_STMNT_ATOMIC? dynamic_cast<stmntAtomic*>(opt->getBlock())->getBlock() : opt->getBlock();
//         auto trans = start->createfsmEdge(node->getLineNb(), new stmntExpr(new exprSkip(node->getLineNb()), node->getLineNb()), nullptr, true);
//         looseEnds.push_back(trans);
        
//         opt->acceptVisitor(this);

//         assert(!looseFeatures);

//         assert(trans->getTargetNode());
//         for(auto t : trans->getTargetNode()->getEdges()) {
//             t->setSourceNode(start);
//         }
//         res->deleteNode(trans->getTargetNode());
        
        
//         flowLooseEnds.merge(looseEnds);
//         looseEnds.clear();
//         flowLooseBreaks.merge(looseBreaks);
//         looseBreaks.clear();

//         opt = opt->getNextOpt();
//     }

//     assert(looseEnds.size() == 0);
//     looseEnds = flowLooseEnds;
//     _connect(start);

//     //looseFeatures = looseFeaturesSave;
//     //elseFeatures = elseFeaturesSave;
//     //hasElseFeatures = hasElseFeaturesSave;

//     /******************************/

//     looseEnds.merge(flowLooseBreaks);

//     auto next = node->getNext();
//     if(next) {
//         next->acceptVisitor(this);
//     }
// }

// void FSMtoAST::visit(const stmntSeq* node) {
//     node->getBlock()->acceptVisitor(this);
// }

// void FSMtoAST::visit(const stmntBreak* node)  {
    

//     current = res->createFsmNode(flags, node->getLineNb());

//     if(!init) init = current;
    
//     _label(current);

//     _connect(current);

//     _looseBreak(node);


//     auto next = node->getNext();
//     if(next) {
//         next->acceptVisitor(this);
//     }
// }

// void FSMtoAST::visit(const stmntGoto* node)  {

//     current = (res->createFsmNode(flags, node->getLineNb()));

//     if(!init) init = current;

//     _connect(current);

//     looseGotos[node->getLabel()].push_back(current->createfsmEdge(node->getLineNb(), node));

//     auto next = node->getNext();
//     if(next) {
//         next->acceptVisitor(this);
//     }
// }

// void FSMtoAST::visit(const stmntLabel* node)  {
//     std::string label = node->getLabel();
    
//     if(label.find("accept") != std::string::npos) 		
//         flags |= fsmNode::N_ACCEPT;
//     if(label.find("end") != std::string::npos) 			
//         flags |= fsmNode::N_END;
//     if(label.find("progress") != std::string::npos) 
//         flags |= fsmNode::N_PROGRESS;

//     labels.push_back(label);

//     node->getLabelled()->acceptVisitor(this);

//     //BUGGY (e.g., for atomic...)
//     flags = 0;

//     auto next = node->getNext();
//     if(next) {
//         next->acceptVisitor(this);
//     }
// }

// void FSMtoAST::visit(const stmntFct* decl)  {
//     init = nullptr;
//     auto curStmnt = decl->getBlock();
//     curStmnt->acceptVisitor(this);
//     res->addInitNode(decl->getFctName(), init);

//     for(auto looseGotoList : looseGotos){
//         auto labelledNodeIt = labeledNodes.find(looseGotoList.first);
//         assert(labelledNodeIt != labeledNodes.end());
//         auto labelledNode = labelledNodeIt->second;
//         for(auto looseGoto : looseGotoList.second) {
//             for(auto inputs : looseGoto->getSourceNode()->getInputEdges()) {
//                 inputs->setTargetNode(labelledNode);
//             }
//             res->deleteNode(looseGoto->getSourceNode());
//         }
//     }
//     looseGotos.clear();

//     //assert(looseEnds.size() == 1);
//     assert(looseGotos.size() == 0);
//     assert(looseBreaks.size() == 0);
//     looseEnds.clear();
//     looseBreaks.clear();
//     looseGotos.clear();


//     auto next = decl->getNext();
//     if(next) {
//         next->acceptVisitor(this);
//     }
// }

// void FSMtoAST::visit(const varDecl* node) {
//     auto next = node->getNext();
//     if(next) {
//         next->acceptVisitor(this);
//     }
// }

// void FSMtoAST::visit(const chanDecl* node) {
//     auto next = node->getNext();
//     if(next) {
//         next->acceptVisitor(this);
//     }
// }

// void FSMtoAST::visit(const tdefDecl* node) {
//     auto next = node->getNext();
//     if(next) {
//         next->acceptVisitor(this);
//     }
// }

// void FSMtoAST::visit(const mtypeDecl* node) {
//     auto next = node->getNext();
//     if(next) {
//         next->acceptVisitor(this);
//     }
// }

// void FSMtoAST::visit(const stmntAtomic* node)  {
//     flags |= fsmNode::N_ATOMIC;
//     node->getBlock()->acceptVisitor(this);
//     flags &= ~fsmNode::N_ATOMIC;
//     auto next = node->getNext();
//     if(next)
//         next->acceptVisitor(this);
// }

// void FSMtoAST::visit(const stmntDStep* node)  {
//     flags |= fsmNode::N_DETERMINISTIC;
//     node->getBlock()->acceptVisitor(this);
//     flags &= ~fsmNode::N_DETERMINISTIC;
//     auto next = node->getNext();
//     if(next)
//         next->acceptVisitor(this);
// }

// void FSMtoAST::visit(const stmntElse* node) {


//     current = (res->createFsmNode(flags, node->getLineNb()));

//     _label(current);
    
//     _connect(current);
    
//     if(hasElseFeatures) {
//         //auto edge = _looseEnd(new stmntExpr(new exprSkip(node->getLineNb()), node->getLineNb()), true);
//         //edge->setFeatures(elseFeatures);
//         //hasElseFeatures = false;
//         _looseEnd(node);
//     } else 
//         _looseEnd(node);

//     auto next = node->getNext();
//     if(next) {
//         next->acceptVisitor(this);
//     }
// }

// void FSMtoAST::visit(const stmntChanRecv* node) {
//     _toFsm(node);
// }

// void FSMtoAST::visit(const stmntChanSnd* node) {
//     _toFsm(node);
// }

// void FSMtoAST::visit(const stmntAction* node) {
//     _toFsm(node);
// }

// void FSMtoAST::visit(const stmntAsgn* node) {
//     _toFsm(node);
// }

// void FSMtoAST::visit(const stmntIncr* node) {
//     _toFsm(node);
// }

// void FSMtoAST::visit(const stmntDecr* node) {
//     _toFsm(node);
// }

// void FSMtoAST::visit(const stmntPrint* node) {
//     _toFsm(node);
// }

// void FSMtoAST::visit(const stmntPrintm* node) {
//     _toFsm(node);
// }

// void FSMtoAST::visit(const stmntAssert* node) {
//     _toFsm(node);
// }